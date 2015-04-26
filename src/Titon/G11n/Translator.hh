<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\G11n;

use Titon\G11n\Event\DetectedEvent;
use Titon\G11n\Event\DetectingEvent;
use Titon\G11n\Event\GlobalizeEvent;
use Titon\G11n\Exception\MissingLocaleException;
use Titon\Io\PathList;
use Titon\Utility\State\Cookie;
use Titon\Utility\State\Server;
use \Locale as SystemLocale;

/**
 * The `Translator` class handles all the necessary functionality for internationalization and
 * localization. This includes defining which locales to support, loading translators,
 * parsing resource bundles, and initializing environments.
 *
 * @link http://en.wikipedia.org/wiki/IETF_language_tag
 * @link http://en.wikipedia.org/wiki/ISO_639
 * @link http://en.wikipedia.org/wiki/ISO_3166-1
 * @link http://loc.gov/standards/iso639-2/php/code_list.php
 *
 * @package Titon\G11n
 */
class Translator {

    /**
     * Currently active locale based on the client.
     *
     * @var \Titon\G11n\Locale
     */
    protected ?Locale $current;

    /**
     * Fallback locale if none can be found.
     *
     * @var \Titon\G11n\Locale
     */
    protected ?Locale $fallback;

    /**
     * Message loader instance.
     *
     * @var \Titon\G11n\MessageLoader
     */
    protected MessageLoader $loader;

    /**
     * Supported list of locales.
     *
     * @var \Titon\G11n\LocaleMap
     */
    protected LocaleMap $locales = Map {};

    /**
     * Resource lookup paths.
     *
     * @var \Titon\Io\PathList
     */
    protected PathList $paths = Map {};

    /**
     * Store the message loader.
     *
     * @param \Titon\G11n\MessageLoader $loader
     */
    public function __construct(MessageLoader $loader) {
        $this->loader = $loader->setTranslator($this);
    }

    /**
     * Map a locale to be supported during the locale detection and message translation process.
     *
     * @param \Titon\G11n\Locale $locale
     * @return \Titon\G11n\Locale
     */
    public function addLocale(Locale $locale): Locale {
        $code = $locale->getCode();

        if ($this->locales->contains($code)) {
            return $this->locales[$code];
        }

        // Inherit resource paths
        $locale->addResourcePaths($this->getResourcePaths());

        // Set the locale
        $this->locales[$code] = LocaleRegistry::set($locale);

        // Set the parent as well
        if ($parent = $locale->getParentLocale()) {
            $this->addLocale($parent);
        }

        // Set fallback if none defined
        if (!$this->fallback) {
            $this->setFallback($code);
        }

        return $locale;
    }

    /**
     * Add multiple resource lookup paths to pass along to all locales.
     *
     * @param \Titon\Io\PathList $paths
     * @return $this
     */
    public function addResourcePaths(PathList $paths): this {
        $this->paths->addAll($paths);

        return $this;
    }

    /**
     * Gather a list of locale codes and fallback locale codes in descending order starting from the current locale.
     *
     * @return Vector<string>
     */
    public function cascade(): Vector<string> {
        $cycle = [];

        foreach ([$this->current(), $this->getFallback()] as $locale) {
            while ($locale instanceof Locale) {
                $cycle[] = $locale->getCode();

                $locale = $locale->getParentLocale();
            }
        }

        return new Vector(array_unique($cycle));
    }

    /**
     * Return the currently detected locale.
     *
     * @return \Titon\G11n\Locale
     */
    public function current(): ?Locale {
        return $this->current;
    }

    /**
     * Detect which locale to use based on the clients Accept-Language header.
     *
     * @return $this
     * @throws \Titon\G11n\Exception\MissingTranslatorException
     */
    public function detect(): this {
        if (!$this->isEnabled()) {
            return $this;
        }

        $current = null;
        $cookieCode = Cookie::get('locale');
        $acceptLang = Server::get('HTTP_ACCEPT_LANGUAGE');

        $this->emit(new DetectingEvent($this));

        // Determine via cookie
        if ($cookieCode && $this->locales->contains($cookieCode)) {
            $current = $cookieCode;

        // Determine locale based on HTTP headers
        } else if ($acceptLang) {
            $header = mb_strtolower($acceptLang);

            if (mb_strpos($header, ';') !== false) {
                $header = mb_strstr($header, ';', true);
            }

            foreach (explode(',', $header) as $code) {
                if ($this->locales->contains($code)) {
                    $current = $code;
                    break;
                }
            }
        }

        // Set current to the fallback if none found
        if ($current === null) {
            $current = $this->getFallback()->getCode();
        }

        $this->emit(new DetectedEvent($this, $current));

        // Apply the locale
        $this->globalize($current);

        return $this;
    }

    /**
     * Return the fallback locale.
     *
     * @return \Titon\G11n\Locale
     */
    public function getFallback(): ?Locale {
        return $this->fallback;
    }

    /**
     * Return a locale by code.
     *
     * @param string $code
     * @return \Titon\G11n\Locale
     * @throws \Titon\G11n\Exception\MissingLocaleException
     */
    public function getLocale(string $code): Locale {
        if ($this->locales->contains($code)) {
            return $this->locales[$code];
        }

        throw new MissingLocaleException(sprintf('Locale %s does not exist', $code));
    }

    /**
     * Return a list of supported locales.
     *
     * @return \Titon\G11n\LocaleMap
     */
    public function getLocales(): LocaleMap {
        return $this->locales;
    }

    /**
     * Return the message loader.
     *
     * @return \Titon\G11n\MessageLoader
     */
    public function getMessageLoader(): MessageLoader {
        return $this->loader;
    }

    /**
     * Return all resource paths.
     *
     * @return \Titon\Io\PathList
     */
    public function getResourcePaths(): PathList {
        return $this->paths;
    }

    /**
     * Set the system and application locale using the built in `setlocale()`.
     *
     * @link http://php.net/setlocale
     * @link http://php.net/manual/locale.setdefault.php
     *
     * @uses Titon\Utility\Config
     *
     * @param string $code
     * @return \Titon\G11n\Locale
     */
    public function globalize(string $code): Locale {
        $newLocale = $this->getLocale(Locale::canonicalize($code));
        $locales = [$newLocale];
        $options = [];

        // Inherit from fallback if different than the new locale
        if ($this->getFallback()->getCode() != $newLocale->getCode()) {
            $locales[] = $this->getFallback();
        }

        // Generate a list of possible system locale names to set
        foreach ($locales as $locale) {
            $meta = $locale->getMetadata();

            $options[] = $meta->getCode() . '.UTF8';
            $options[] = $meta->getCode() . '.UTF-8';
            $options[] = $meta->getCode();

            foreach ($meta->getISO3Code() as $iso3) {
                $options[] = $iso3 . '.UTF8';
                $options[] = $iso3 . '.UTF-8';
                $options[] = $iso3;
            }

            if ($iso2 = $meta->getISO2Code()) {
                $options[] = $iso2 . '.UTF8';
                $options[] = $iso2 . '.UTF-8';
                $options[] = $iso2;
            }
        }

        // Emit change event
        $event = new GlobalizeEvent($this, $newLocale);
        $this->emit($event);
        $newLocale = $event->getLocale();

        // Set system locale
        $code = $newLocale->getCode();

        putenv('LC_ALL=' . $code);
        setlocale(LC_ALL, $options);

        SystemLocale::setDefault($code);

        $this->current = $newLocale;

        Config::set('titon.locale.current', $code);
        Config::set('titon.locale.cascade', $this->cascade());

        return $newLocale;
    }

    /**
     * Does the current locale match the passed code?
     *
     * @param string $code
     * @return bool
     */
    public function is(string $code): bool {
        $currentCode = $this->current()?->getCode();

        return ($currentCode === $code || Locale::canonicalize($code) === $currentCode);
    }

    /**
     * Globalization will be enabled if more than 1 locale has been setup.
     *
     * @return bool
     */
    public function isEnabled(): bool {
        return !$this->locales->isEmpty();
    }

    /**
     * Define the fallback locale to use if none can be found or is not supported.
     *
     * @uses Titon\Utility\Config
     *
     * @param string $code
     * @return $this
     * @throws \Titon\G11n\Exception\MissingLocaleException
     */
    public function setFallback(string $code): this {
        $this->fallback = $this->getLocale($code);

        Config::set('titon.locale.fallback', $code);

        return $this;
    }

    /**
     * Return a translated message from the message catalogs and optional interpolate parameters.
     *
     * @param string $key
     * @param \Titon\G11n\ParamList $params
     * @return string
     */
    public function translate(string $key, ParamList $params = Vector {}): string {
        return $this->getMessageLoader()->translate($key, $params);
    }

}
