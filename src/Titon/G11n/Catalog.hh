<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\G11n;

use Titon\G11n\Exception\InvalidCatalogException;
use Titon\G11n\Exception\MissingMessageException;

/**
 * TODO
 *
 * @package Titon\G11n
 */
class Catalog {

    /**
     * Domain the catalog belongs to.
     *
     * @var string
     */
    protected string $domain;

    /**
     * List of messages within the catalog.
     *
     * @var \Titon\G11n\MessageMap
     */
    protected MessageMap $messages = Map {};

    /**
     * Name of the catalog.
     *
     * @var string
     */
    protected string $name;

    /**
     * Store the domain and catalog name.
     *
     * @param string $name
     * @param string $domain
     */
    public function __construct(string $name, string $domain) {
        $this->name = $name;
        $this->domain = $domain;
    }

    /**
     * Return the domain name.
     *
     * @return string
     */
    public function getDomain(): string {
        return $this->domain;
    }

    /**
     * Return a message from the catalog defined by key.
     *
     * @param string $key
     * @return string
     */
    public function getMessage(string $key): string {
        if ($this->messages->contains($key)) {
            return $this->messages[$key];
        }

        throw new MissingMessageException(sprintf('Message key %s.%s.%s does not exist', $this->getDomain(), $this->getName(), $key));
    }

    /**
     * Return the list of messages.
     *
     * @return \Titon\G11n\MessageMap
     */
    public function getMessages(): MessageMap {
        return $this->messages;
    }

    /**
     * Return the catalog name.
     *
     * @return string
     */
    public function getName(): string {
        return $this->name;
    }

    /**
     * Parse out the domain, catalog, and key for a message lookup.
     *
     * @param string $key
     * @return \Titon\G11n\MessageKey
     * @throws \Titon\G11n\Exception\InvalidCatalogException
     */
    <<__Memoize>>
    public static function parseKey(string $key): MessageKey {
        $parts = explode('.', preg_replace('/[^-_a-z0-9\.]+/i', '', $key));
        $count = count($parts);
        $domain = 'common';

        if ($count < 2) {
            throw new InvalidCatalogException(sprintf('No domain or catalog present for %s key', $key));

        } else if ($count === 2) {
            $catalog = $parts[0];
            $key = $parts[1];

        } else {
            $domain = array_shift($parts);
            $catalog = array_shift($parts);
            $key = implode('.', $parts);
        }

        return shape(
            'domain' => $domain,
            'catalog' => $catalog,
            'key' => $key
        );
    }

    /**
     * Set a list of messages.
     *
     * @param \Titon\G11n\MessageMap $messages
     * @return $this
     */
    public function setMessages(MessageMap $messages): this {
        $this->messages = $messages;

        return $this;
    }

}
