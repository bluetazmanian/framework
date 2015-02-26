<?hh
namespace Titon\Environment;

use Titon\Test\Stub\Environment\BootstrapperStub;
use Titon\Test\Stub\Environment\EnvironmentStub;
use Titon\Test\TestCase;
use Titon\Utility\State\Server as ServerGlobal;

/**
 * @property \Titon\Environment\Environment $object
 */
class EnvironmentTest extends TestCase {

    protected function setUp(): void {
        parent::setUp();

        $this->object = new EnvironmentStub();
        $this->object->addBootstrapper(new BootstrapperStub());
        $this->object->addHost('dev', new Host(Server::DEV, ['dev', '123.0.0.0']));
        $this->object->addHost('prod', new Host(Server::PROD, Vector {'prod', '123.456.0.0'}));
        $this->object->addHost('staging', new Host(Server::STAGING, Vector {'staging', '123.456.789.0'}));
        $this->object->setFallback('dev');
    }

    public function testBootstrappers(): void {
        $this->assertEquals(1, count($this->object->getBootstrappers()));

        $bs = new BootstrapperStub();
        $this->object->addBootstrapper($bs);

        $this->assertEquals(2, count($this->object->getBootstrappers()));
        $this->assertEquals($bs, $this->object->getBootstrappers()[1]);
    }

    public function testCurrent(): void {
        // dev
        $_SERVER['HTTP_HOST'] = 'dev';

        $this->object->initialize();
        $this->assertInstanceOf('Titon\Environment\Host', $this->object->current());
        $this->assertEquals('dev', $this->object->current()->getKey());
        $this->assertEquals(Server::DEV, $this->object->current()->getType());

        // dev ip
        $_SERVER['HTTP_HOST'] = '123.0.0.0';

        $this->object->initialize();
        $this->assertEquals('dev', $this->object->current()->getKey());

        // prod
        $_SERVER['HTTP_HOST'] = 'prod';

        $this->object->initialize();
        $this->assertEquals('prod', $this->object->current()->getKey());
        $this->assertEquals(Server::PROD, $this->object->current()->getType());

        // prod ip
        $_SERVER['HTTP_HOST'] = '123.456.0.0';

        $this->object->initialize();
        $this->assertEquals('prod', $this->object->current()->getKey());

        // staging
        $_SERVER['HTTP_HOST'] = 'staging';

        $this->object->initialize();
        $this->assertEquals('staging', $this->object->current()->getKey());
        $this->assertEquals(Server::STAGING, $this->object->current()->getType());

        // staging ip
        $_SERVER['HTTP_HOST'] = '123.456.789.0';

        $this->object->initialize();
        $this->assertEquals('staging', $this->object->current()->getKey());

        // test SERVER_ADDR over HTTP_HOST
        $_SERVER['HTTP_HOST'] = '';
        $_SERVER['SERVER_ADDR'] = '123.0.0.0';

        $this->object->initialize();
        $this->assertEquals('dev', $this->object->current()->getKey());
    }

    public function testSetFallback(): void {
        $this->object->setFallback('dev');

        $_SERVER['HTTP_HOST'] = 'fake_environment';
        $_SERVER['SERVER_ADDR'] = '0.0.0.0';

        $this->object->initialize();
        $this->assertEquals('dev', $this->object->current()->getKey());
        $this->assertEquals('dev', $this->object->getFallback()->getKey());
    }

    /**
     * @expectedException \Titon\Environment\Exception\MissingHostException
     */
    public function testSetFallbackMissingHost(): void {
        $this->object->setFallback('fakeEnv');
    }

    public function testGetHosts(): void {
        $this->assertEquals(3, count($this->object->getHosts()));
    }

    public function testBootstrapping(): void {
        $_SERVER['HTTP_HOST'] = 'dev';

        $this->object->initialize();
        $this->assertEquals('dev', BootstrapperStub::$loaded);

        $_SERVER['HTTP_HOST'] = 'prod';

        $this->object->initialize();
        $this->assertEquals('prod', BootstrapperStub::$loaded);
    }

    public function testBootstrappingFallback(): void {
        $_SERVER['HTTP_HOST'] = 'qa';

        $this->object->initialize();
        $this->assertEquals('dev', BootstrapperStub::$loaded); // Since dev is the fallback
    }

    public function testInitializeNoHosts(): void {
        $env = new EnvironmentStub();
        $env->initialize();

        $this->assertEquals(null, $env->current());
    }

    public function testIs(): void {
        // dev
        $_SERVER['HTTP_HOST'] = 'dev';

        $this->object->initialize();
        $this->assertTrue($this->object->is('dev'));
        $this->assertFalse($this->object->is('prod'));

        // dev ip
        $_SERVER['HTTP_HOST'] = '123.0.0.0';

        $this->object->initialize();
        $this->assertTrue($this->object->is('dev'));
        $this->assertFalse($this->object->is('prod'));

        // prod
        $_SERVER['HTTP_HOST'] = 'prod';

        $this->object->initialize();
        $this->assertTrue($this->object->is('prod'));
        $this->assertFalse($this->object->is('staging'));

        // prod ip
        $_SERVER['HTTP_HOST'] = '123.456.0.0';

        $this->object->initialize();
        $this->assertTrue($this->object->is('prod'));
        $this->assertFalse($this->object->is('staging'));

        // staging
        $_SERVER['HTTP_HOST'] = 'staging';

        $this->object->initialize();
        $this->assertTrue($this->object->is('staging'));
        $this->assertFalse($this->object->is('dev'));

        // staging ip
        $_SERVER['HTTP_HOST'] = '123.456.789.0';

        $this->object->initialize();
        $this->assertTrue($this->object->is('staging'));
        $this->assertFalse($this->object->is('dev'));

        // test SERVER_ADDR over HTTP_HOST
        $_SERVER['HTTP_HOST'] = '';
        $_SERVER['SERVER_ADDR'] = '123.0.0.0';

        $this->object->initialize();
        $this->assertTrue($this->object->is('dev'));
        $this->assertFalse($this->object->is('staging'));
    }

    public function testIsMachine(): void {
        if (getenv('TRAVIS')) {
            $this->markTestSkipped('Can\'t test host names within travis as they are too different per box');
        }

        $env = new Environment();

        $this->assertTrue($env->isMachine('titon'));
        $this->assertTrue($env->isMachine('tit*'));
    }

    public function testIsLocalhost(): void {
        $_SERVER['HTTP_HOST'] = 'domain.com';
        $_SERVER['REMOTE_ADDR'] = '127.33.123.54';
        ServerGlobal::initialize($_SERVER);

        $this->assertFalse($this->object->isLocalhost());

        $_SERVER['REMOTE_ADDR'] = '127.0.0.1';
        ServerGlobal::initialize($_SERVER);

        $this->assertTrue($this->object->isLocalhost());

        $_SERVER['REMOTE_ADDR'] = '::1';
        ServerGlobal::initialize($_SERVER);

        $this->assertTrue($this->object->isLocalhost());

        $_SERVER['HTTP_HOST'] = 'localhost';
        $_SERVER['REMOTE_ADDR'] = '127.33.123.54';
        ServerGlobal::initialize($_SERVER);

        $this->assertTrue($this->object->isLocalhost());
    }

    public function testIsDevelopment(): void {
        $_SERVER['HTTP_HOST'] = 'dev';
        ServerGlobal::initialize($_SERVER);

        $this->object->initialize();
        $this->assertTrue($this->object->isDevelopment());
        $this->assertFalse($this->object->isProduction());
        $this->assertFalse($this->object->isQA());
        $this->assertFalse($this->object->isStaging());
    }

    public function testIsProduction(): void {
        $_SERVER['HTTP_HOST'] = 'prod';

        $this->object->initialize();
        $this->assertFalse($this->object->isDevelopment());
        $this->assertTrue($this->object->isProduction());
        $this->assertFalse($this->object->isQA());
        $this->assertFalse($this->object->isStaging());
    }

    public function testIsQA(): void {
        $this->object->addHost('qa', new Host(Server::QA, ['qa', '123.456.0.666']));

        $_SERVER['HTTP_HOST'] = 'qa';

        $this->object->initialize();
        $this->assertFalse($this->object->isDevelopment());
        $this->assertFalse($this->object->isProduction());
        $this->assertTrue($this->object->isQA());
        $this->assertFalse($this->object->isStaging());
    }

    public function testIsStaging(): void {
        $_SERVER['HTTP_HOST'] = 'staging';

        $this->object->initialize();
        $this->assertFalse($this->object->isDevelopment());
        $this->assertFalse($this->object->isProduction());
        $this->assertFalse($this->object->isQA());
        $this->assertTrue($this->object->isStaging());
    }

    public function testVarLoading(): void {
        $_SERVER['HTTP_HOST'] = 'dev';

        $env = new EnvironmentStub(TEMP_DIR . '/environment/');
        $env->addHost('dev', new Host(Server::DEV, ['dev', '123.0.0.0']));

        $this->assertEquals(Map {}, $env->getVariables());
        $this->assertEquals('', $env->getVariable('bar'));

        $env->initialize();

        $this->assertEquals(Map {'foo' => 'bar'}, $env->getVariables());
        $this->assertEquals('bar', $env->getVariable('foo'));
        $this->assertEquals('', $env->getVariable('bar'));
    }

    public function testVarLoadingInheritance(): void {
        $_SERVER['HTTP_HOST'] = 'prod';

        $env = new EnvironmentStub(TEMP_DIR . '/environment/');
        $env->addHost('prod', new Host(Server::PROD, Vector {'prod', '123.456.0.0'}));
        $env->initialize();

        $this->assertEquals(Map {'foo' => 'bar', 'baz' => 'qux'}, $env->getVariables());
        $this->assertEquals('bar', $env->getVariable('foo'));
        $this->assertEquals('qux', $env->getVariable('baz'));
        $this->assertEquals('', $env->getVariable('bar'));
    }

}
