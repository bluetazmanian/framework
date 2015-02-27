<?hh
namespace Titon\Common;

use Titon\Test\TestCase;
use Titon\Utility\Crypt;
use Titon\Utility\Format;
use Titon\Utility\Inflector;
use Titon\Utility\Number;
use Titon\Utility\Path;

class MacroableTest extends TestCase {

    public function testHasMacro(): void {
        $this->assertFalse(Number::hasMacro('toBinary'));
        $this->assertFalse(Number::hasMacro('toFloat'));

        Number::macro('toFloat', function() {
            return 0.0;
        });

        $this->assertFalse(Number::hasMacro('toBinary'));
        $this->assertTrue(Number::hasMacro('toFloat'));
    }

    public function testInheritance(): void {
        $this->assertFalse(Format::hasMacro('foobar'));
        $this->assertFalse(Path::hasMacro('foobar'));

        Format::macro('foobar', function() {
            return '';
        });

        $this->assertTrue(Format::hasMacro('foobar'));
        $this->assertFalse(Path::hasMacro('foobar'));
    }

    public function testMacro(): void {
        Inflector::macro('caps', function($value) {
            return strtoupper($value);
        });

        $this->assertEquals('FOOBAR', Inflector::caps('foObAr'));
    }

    /**
     * @expectedException \Titon\Common\Exception\MissingMacroException
     */
    public function testMacroMissing(): void {
        Inflector::lowers('foObAr');
    }

    public function testGetMacros(): void {
        $lower = function($value) {
            return strtolower($value);
        };

        $upper = function($value) {
            return strtoupper($value);
        };

        Crypt::macro('lower', $lower);
        Crypt::macro('upper', $upper);

        $this->assertEquals(Map {
            'lower' => $lower,
            'upper' => $upper
        }, Crypt::getMacros());
    }

}
