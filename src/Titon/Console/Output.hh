<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console;

use Exception;
use Titon\Console\System\SystemFactory;

/**
 * The `Output` class handles formatting and sending data to the ouput stream.
 *
 * @package Titon\Console
 */
class Output {

    const int VERBOSITY_QUIET = 0;
    const int VERBOSITY_NORMAL = 1;
    const int VERBOSITY_VERBOSE = 3;
    const string STREAM_STDOUT = 'php://stdout';
    const string STREAM_STDERR = 'php://stderr';
    const string ANSI_CLEAR_LINE = "\033[K";
    const string LF = PHP_EOL;
    const string CR = "\r";

    /**
     * Flag to determine if we should always output ANSI.
     *
     * @var bool
     */
    protected bool $forceAnsi = false;

    /**
     * The singleton instance.
     *
     * @var \Titon\Console\Output|null
     */
    protected static ?Output $instance;

    /**
     * The resource handle for standard error.
     *
     * @var resource
     */
    protected resource $stderr;

    /**
     * The resource handle for standard output.
     *
     * @var resource
     */
    protected resource $stdout;

    /**
     * Data structure containing available styles to apply to output.
     *
     * @var \Titon\Console\StyleMap
     */
    protected StyleMap $styles = Map {};

    /**
     * Flag to determine if we should never output ANSI.
     *
     * @var bool
     */
    protected bool $suppressAnsi = false;

    /**
     * The global verbosity level for the `Output`.
     *
     * @var int
     */
    protected int $verbosity;

    /**
     * Construct a new `Output` object.
     *
     * @param int $verbosity    The verbosity level for the `Output`
     */
    public function __construct(int $verbosity = self::VERBOSITY_NORMAL) {
        $this->stderr = fopen(Output::STREAM_STDERR, 'w');
        $this->stdout = fopen(Output::STREAM_STDOUT, 'w');
        $this->verbosity = $verbosity;

        if (is_null(self::$instance)) {
            self::$instance = $this;
        }
    }

    /**
     * Remove a specific element's style.
     *
     * @param string $element   The element to remove
     *
     * @return $this
     */
    public function clearStyle(string $element): this {
        $this->styles->remove($element);

        return $this;
    }

    /**
     * Send output to the error stream.
     *
     * @param string $output        The contents to output
     * @param int    $verbosity     The verbosity level of the output
     * @param int    $newLines      The number of new lines to append to the output
     * @param string $newlineChar   The new line character to use
     */
    public function error(string $output = '', int $verbosity = Output::VERBOSITY_NORMAL, int $newLines = 1, string $newlineChar = Output::LF): void {
        if (!$this->shouldOutput($verbosity)) {
            return;
        }

        $output = $this->format($output);
        $output .= str_repeat("\n", $newLines);

        $this->send($output, $this->stderr);
    }

    /**
     * Set flag if ANSI output should be forced.
     *
     * @param bool $suppress    If ANSI should be forced
     *
     * @return $this
     */
        public function setForceAnsi(bool $force): this {
        $this->forceAnsi = $force;

        return $this;
    }

    /**
     * Format contents by parsing the style tags and applying necessary formatting.
     *
     * @param string $message   The message to format
     *
     * @string
     */
    public function format(string $message): string {
        $parsedTags = array_unique($this->parseTags($message));
        $retval = $message;
        foreach ($parsedTags as $xmlTag) {
            if (!is_null($this->styles->get($xmlTag))) {
                $outputAnsi = false;
                if ($this->forceAnsi === true) {
                    $outputAnsi = true;
                } else if ($this->suppressAnsi === true) {
                    $outputAnsi = false;
                } else if (SystemFactory::factory()->supportsAnsi() === true) {
                    $outputAnsi = true;
                }

                $style = $this->styles[$xmlTag];
                $retval = $style->format($xmlTag, $retval, $outputAnsi);

                $matches = [];
                $retval = preg_replace_callback('#<[\w-]+?>.*<\/[\w-]+?>#', ($matches) ==> {
                    if ($outputAnsi === true) {
                        return sprintf("%s%s%s", $style->getEndCode(), $this->format($matches[0]), $style->getStartCode());
                    }

                    return sprintf("%s", $this->format($matches[0]));
                }, $retval);
            }
        }

        return $retval;
    }

    /**
     * Create and return the singleton instance.
     *
     * @return \Titon\Console\Output
     */
    public static function getInstance(): Output {
        if (is_null(self::$instance)) {
            self::$instance = new Output();
        }

        return self::$instance;
    }

    /**
     * Send output to the standard output stream.
     *
     * @param string $output        The contents to output
     * @param int    $verbosity     The verbosity level of the output
     * @param int    $newLines      The number of new lines to append to the output
     * @param string $newlineChar   The new line character to use
     */
    public function out(string $output = '', int $verbosity = Output::VERBOSITY_NORMAL, int $newLines = 1, string $newlineChar = Output::LF): void {
        if (!$this->shouldOutput($verbosity)) {
            return;
        }

        $output = $this->format($output);
        $output .= str_repeat($newlineChar, $newLines);

        $this->send($output, $this->stdout);
    }

    /**
     * Parse all available style tags from the given contents.
     *
     * @param string $stringToParse The contents to parse
     *
     * @return array<string>
     */
    protected function parseTags(string $stringToParse): array<string> {
        $tagsMatched = [];
        preg_match_all('#<([\w-]*?)>#', $stringToParse, $tagsMatched);

        return $tagsMatched[1];
    }

    /**
     * Basic `Exception` renderer to handle outputting of uncaught exceptions
     * thrown in `Command` objects.
     *
     * @param Exception $exception  The `Exception` thrown
     */
    public function renderException(Exception $exception): void {
        $class = explode("\\", get_class($exception));
        $class = $class[count($class) - 1];

        $message = explode('{{BREAK}}', wordwrap($exception->getMessage(), 40, "{{BREAK}}"));
        array_unshift($message, "[$class]");

        $length = max(array_map(($key) ==> {
            return strlen($key);
        }, $message));

        $this->error(Output::LF);
        $this->error("<exception>  " . str_pad("", $length) . "  </exception>");

        foreach ($message as $line) {
            $line = str_pad($line, $length);
            $this->error("<exception>  $line  </exception>");
        }

        $this->error("<exception>  " . str_pad("", $length) . "  </exception>");
        $this->error(Output::LF);
    }

    /**
     * Send the response through the given output stream.
     *
     * @param string   $response    The contents to send
     * @param resource $stream      The output stream
     */
    protected function send(string $response, resource $stream): void {
        fwrite($stream, $response);
    }

    /**
     * Assign a `StyleDefinition` to an XML tag.
     *
     * @param string          $element  The element to assign
     * @param StyleDefinition $format   The style to apply to the given tag
     */
    public function setStyle(string $element, StyleDefinition $format): this {
        $this->styles[$element] = $format;

        return $this;
    }

    /**
     * Set flag if ANSI output should be suppressed.
     *
     * @param bool $suppress    If ANSI should be suppressed
     *
     * @return $this
     */
    public function setSuppressAnsi(bool $suppress): this {
        $this->suppressAnsi = $suppress;

        return $this;
    }

    /**
     * Set the global verbosity of the `Output`.
     *
     * @param int $verbosity    The verbosity to set
     *
     * @return $this
     */
    public function setVerbosity(int $verbosity): this {
        $this->verbosity = $verbosity;

        return $this;
    }

    /**
     * Determine how the given verbosity compares to the class's verbosity level.
     *
     * @param int $verbosity    The verbosity to check
     *
     * @return bool
     */
    protected function shouldOutput(int $verbosity): bool {
        if ($verbosity <= $this->verbosity) {
            return true;
        }

        return false;
    }
}