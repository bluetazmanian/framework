<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console\InputDefinition;

class Argument extends AbstractInputDefinition {

    public function setValue(string $value): this {
        $this->value = $value;

        return $this;
    }
}