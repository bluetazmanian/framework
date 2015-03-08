<?hh

namespace Titon\Console;

use Titon\Test\TestCase;
use Titon\Test\Stub\Console\CommandStub;
use Titon\Console\InputDefinition\Flag;
use Titon\Console\InputDefinition\Option;
use Titon\Console\InputDefinition\Argument;

class CommandTest extends TestCase {

    public function testAddArguments(): void {
        $args = new Input([
            '-v'
        ]);
        $args->addFlag(new Flag('help', 'Show this help screen'));

        $command = new CommandStub();
        $command->setInput($args);
        $command->configure();
        $command->registerInput();

        $args->parse();

        $this->assertEquals(2, count($args->getFlags()));
        $this->assertEquals(1, $args->getFlag('v')->getValue());
    }
}