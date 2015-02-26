<?hh // strict
namespace Titon\Type;

use Titon\Test\TestCase;

/**
 * @property \Titon\Type\StringBuffer $object
 */
class StringTest extends TestCase {

    protected function setUp(): void {
        parent::setUp();

        $this->object = new StringBuffer('titon');
    }

    public function testAppend(): void {
        $this->assertEquals('titon framework', $this->object->append(' framework')->value());
    }

    public function testCapitalize(): void {
        $this->assertEquals('Titon', $this->object->capitalize()->value());
    }

    public function testCharAt(): void {
        $this->assertEquals('o', $this->object->charAt(3));
        $this->assertEquals(null, $this->object->charAt(10));
    }

    public function testClean(): void {
        $string = new StringBuffer('  titon       framework' . PHP_EOL);

        $this->assertEquals('titon framework', $string->clean()->value());
    }

    public function testCompare(): void {
        $this->assertEquals(0, $this->object->compare('titon'));
        $this->assertLessThan(0, $this->object->compare('titon framework'));
        $this->assertGreaterThan(0, $this->object->compare('tit'));
    }

    public function testConcat(): void {
        $append = $this->object->concat(' append');

        $this->assertEquals('titon append', $append->value());

        $prepend = $this->object->concat('prepend ', false);

        $this->assertEquals('prepend titon', $prepend->value());
    }

    public function testContains(): void {
        $this->assertTrue($this->object->contains('tit'));
        $this->assertFalse($this->object->contains('tin'));
    }

    public function testEndsWith(): void {
        $this->assertTrue($this->object->endsWith('ton'));
        $this->assertFalse($this->object->endsWith('tan'));
    }

    public function testEquals(): void {
        $this->assertTrue($this->object->equals('titon'));
        $this->assertFalse($this->object->equals('titan'));
    }

    public function testEscape(): void {
        $string = new StringBuffer('"Titon"');

        $this->assertEquals('&quot;Titon&quot;', $string->escape()->value());
    }

    public function testExtract(): void {
        $this->assertEquals('tit', $this->object->extract(0, 3));
        $this->assertEquals('ton', $this->object->extract(2));
        $this->assertEquals('on', $this->object->extract(-2));
    }

    public function testIndexOf(): void {
        $this->assertEquals(0, $this->object->indexOf('t'));
        $this->assertEquals(4, $this->object->indexOf('n'));
        $this->assertEquals(-1, $this->object->indexOf('r'));
    }

    public function testIsEmpty(): void {
        $string = new StringBuffer('');

        $this->assertTrue($string->isEmpty());

        $string->write('foobar');

        $this->assertFalse($string->isEmpty());
    }

    public function testLastIndexOf(): void {
        $this->assertEquals(2, $this->object->lastIndexOf('t'));
        $this->assertEquals(4, $this->object->lastIndexOf('n'));
        $this->assertEquals(-1, $this->object->lastIndexOf('r'));
    }

    public function testLength(): void {
        $this->assertEquals(5, $this->object->length());

        $this->assertEquals(15, $this->object->append(' framework')->length());
    }

    public function testMatches(): void {
        $this->assertEquals(1, $this->object->matches('/tit/'));
        $this->assertEquals(['tit'], $this->object->matches('/tit/', true));
        $this->assertEquals(0, $this->object->matches('/tat/'));
    }

    public function testPad(): void {
        $this->assertEquals('--titon---', $this->object->pad(10, '-'));
    }

    public function testPadLeft(): void {
        $this->assertEquals('-----titon', $this->object->padLeft(10, '-'));
    }

    public function testPadRight(): void {
        $this->assertEquals('titon-----', $this->object->padRight(10, '-'));
    }

    public function testPrepend(): void {
        $this->assertEquals('php framework: titon', $this->object->prepend('php framework: ')->value());
    }

    public function testReplace(): void {
        $object = $this->object->replace('tit', 'TIT');
        $this->assertEquals('TITon', $object->value());

        $object = $object->replace('tit', 'tit');
        $this->assertEquals('TITon', $object->value());

        $object = $object->replace('tit', 'tit', false);
        $this->assertEquals('titon', $object->value());
    }

    public function testReverse(): void {
        $this->assertEquals('notit', $this->object->reverse()->value());
    }

    public function testSerialize(): void {
        $serialized = serialize($this->object);

        $this->assertEquals('C:23:"Titon\Type\StringBuffer":12:{s:5:"titon";}', $serialized);
        $this->assertEquals($this->object, unserialize($serialized));
    }

    public function testShuffle(): void {
        $this->object->append(' framework');
        $this->object->shuffle();
        $this->assertNotEquals('titon framework', $this->object->value());
    }

    public function testStartsWith(): void {
        $this->assertTrue($this->object->startsWith('tit'));
        $this->assertFalse($this->object->startsWith('tot'));
    }

    public function testStrip(): void {
        $this->assertEquals('titon', $this->object->write('<b>titon</b>')->strip()->value());
    }

    public function testSplit(): void {
        $this->assertEquals(Vector {'t', 'i', 't', 'o', 'n'}, $this->object->split());
        $this->assertEquals(Vector {'ti', 'to', 'n'}, $this->object->split('', 2));
        $this->assertEquals(Vector {'', 'i', 'on'}, $this->object->split('t'));
        $this->assertEquals(Vector {'', 'iton'}, $this->object->split('t', 2));
    }

    public function testToCamelCase(): void {
        $this->assertEquals('TitonFramework', $this->object->write('titon framework')->toCamelCase()->value());
    }

    public function testToLowerCase(): void {
        $this->assertEquals('titon framework', $this->object->write('TITON fRamEwork')->toLowerCase()->value());
    }

    public function testToUpperCase(): void {
        $this->assertEquals('TITON FRAMEWORK', $this->object->write('titon framework')->toUpperCase()->value());
    }

    public function testToUpperWords(): void {
        $this->assertEquals('Titon Framework', $this->object->write('titon framework')->toUpperWords()->value());
    }

    public function testTrim(): void {
        $this->assertEquals('iton', $this->object->trim('t')->value());
        $this->assertEquals('titon', $this->object->write(' titon ')->trim()->value());
    }

    public function testTrimLeft(): void {
        $this->assertEquals('titon ', $this->object->write(' titon ')->trimLeft()->value());
        $this->assertEquals('titon-', $this->object->write('-titon-')->trimLeft('-')->value());
    }

    public function testTrimRight(): void {
        $this->assertEquals(' titon', $this->object->write(' titon ')->trimRight()->value());
        $this->assertEquals('-titon', $this->object->write('-titon-')->trimRight('-')->value());
    }

    public function testUncapitalize(): void {
        $this->assertEquals('titon Framework', $this->object->write('Titon Framework')->uncapitalize()->value());
    }

    public function testWordCount(): void {
        $this->assertEquals(1, $this->object->wordCount());

        $this->object->write('Titon Framework');
        $this->assertEquals(2, $this->object->wordCount());
    }

    public function testWrite(): void {
        $this->object->write('framework');
        $this->assertEquals('framework', $this->object->value());
    }

}
