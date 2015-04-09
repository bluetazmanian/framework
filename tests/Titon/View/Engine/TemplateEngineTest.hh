<?hh
namespace Titon\View\Engine;

use Titon\Test\TestCase;
use Titon\View\EngineView;
use Titon\View\Locator\TemplateLocator;
use Titon\View\ViewTest;

/**
 * @property \Titon\View\EngineView $object
 * @property \Titon\View\Engine\TemplateEngine $engine
 */
class TemplateEngineTest extends TestCase {

    protected function setUp(): void {
        parent::setUp();

        $this->vfs()->createStructure(ViewTest::generateViewStructure());

        $this->engine = new TemplateEngine();

        $this->object = new EngineView(new TemplateLocator($this->vfs()->path('/views')));
        $this->object->setEngine($this->engine);
    }

    public function testOpen(): void {
        $this->assertEquals('nested/include.tpl', $this->object->getEngine()->open('nested/include'));
        $this->assertEquals('nested/include.tpl', $this->object->getEngine()->open('nested/include.tpl'));
        $this->assertEquals('Titon - partial - variables.tpl', $this->object->getEngine()->open('variables', Map {
            'name' => 'Titon',
            'type' => 'partial',
            'filename' => 'variables.tpl'
        }));
    }

    /**
     * @expectedException \Titon\View\Exception\MissingTemplateException
     */
    public function testOpenMissingFile(): void {
        $this->object->getEngine()->open('foobar');
    }

    public function testRender(): void {
        $this->assertEquals('add.tpl', $this->object->renderTemplate($this->object->getLocator()->locate('index/add')));
        $this->assertEquals('test-include.tpl nested/include.tpl', $this->object->renderTemplate($this->object->getLocator()->locate('index/test-include')));
    }

    public function testData(): void {
        $this->assertEquals('add.tpl', $this->engine->render($this->object->getLocator()->locate('index/add'), Map {
            'foo' => 'bar'
        }));

        $this->assertEquals('bar', $this->engine->data('foo'));
        $this->assertEquals(null, $this->engine->data('key'));
        $this->assertEquals('default', $this->engine->data('key', 'default'));
    }

}
