<?hh
namespace Titon\Route;

use Titon\Cache\Storage\MemoryStorage;
use Titon\Test\Stub\Route\FilterStub;
use Titon\Test\Stub\Route\TestRouteStub;
use Titon\Test\TestCase;
use Titon\Utility\State\Get;
use Titon\Utility\State\Server;
use Titon\Context\Depository;

/**
 * @property \Titon\Route\Router $object
 */
class RouterTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        $container = Depository::getInstance();
        $container->singleton('Titon\Route\Router');

        $this->object = Depository::getInstance()->make('Titon\Route\Router');
        $container->register('Titon\Route\UrlBuilder')->with($this->object);

        $this->object->map('action.ext', new TestRouteStub('/{module}/{controller}/{action}.{ext}', 'Module\Controller@action'));
        $this->object->map('action', new TestRouteStub('/{module}/{controller}/{action}', 'Module\Controller@action'));
        $this->object->map('controller', new TestRouteStub('/{module}/{controller}', 'Module\Controller@action'));
        $this->object->map('module', new TestRouteStub('/{module}', 'Module\Controller@action'));
        $this->object->map('root', new TestRouteStub('/', 'Module\Controller@action'));
    }

    protected function tearDown() {
        Depository::getInstance()->remove('Titon\Route\Router');
        Depository::getInstance()->remove('Titon\Route\UrlBuilder');
    }

    public function testBuildAction() {
        $this->assertEquals('Controller@action', Router::buildAction(shape('class' => 'Controller', 'action' => 'action')));
    }

    public function testCaching() {
        $storage = new MemoryStorage();
        $route1 = new Route('/{module}', 'Module\Controller@action');
        $route2 = new Route('/', 'Module\Controller@action');

        $router1 = new Router();
        $router1->setStorage($storage);
        $router1->map('module', $route1);
        $router1->map('root', $route2);

        $this->assertFalse($storage->has('routes'));

        $router1->initialize();

        $this->assertTrue($storage->has('routes'));

        // Now load another instance

        $router2 = new Router();
        $router2->setStorage($storage);

        $this->assertEquals(Map {}, $router2->getRoutes());

        $router2->map('root', new Route('/foobar', 'Module\Controller@action'));
        $router2->initialize();

        $this->assertEquals(Map {'module' => $route1, 'root' => $route2}, $router2->getRoutes());

        // The previous routes should be overwritten
        $this->assertEquals('/', $router2->getRoute('root')->getPath());
    }

    public function testFilters() {
        $stub = new FilterStub();

        $this->object->filter('test', $stub);
        $this->object->filterCallback('test2', function() {});

        $this->assertEquals(inst_meth($stub, 'filter'), $this->object->getFilter('test'));
        $this->assertEquals(Vector {'test', 'test2'}, $this->object->getFilters()->keys());

        // Filtering is passed to routes
        $this->object->map('f1', (new Route('/f1', 'Controller@action'))->addFilter('test2'));
        $this->object->group(function(Router $router, Group $group) {
            $group->setFilters(Vector {'test'});

            $router->map('f2', new Route('/f2', 'Controller@action'));
        });
        $this->object->map('f3', new Route('/f3', 'Controller@action'));

        $routes = $this->object->getRoutes();

        $this->assertEquals(Vector {'test2'}, $routes['f1']->getFilters());
        $this->assertEquals(Vector {'test'}, $routes['f2']->getFilters());
        $this->assertEquals(Vector {}, $routes['f3']->getFilters());
    }

    /**
     * @expectedException \Titon\Route\Exception\MissingFilterException
     */
    public function testFilterMissingKey() {
        $this->object->getFilter('fakeKey');
    }

    public function testFilterIsTriggered() {
        $router = new Router();
        $count = 0;

        $router->filterCallback('test', function() use (&$count) {
            $count++;
        });

        $router->map('f1', (new Route('/f1', 'Controller@action'))->addFilter('test'));
        $router->group(function(Router $router, Group $group) {
            $group->setFilters(Vector {'test'});

            $router->map('f2', new Route('/f2', 'Controller@action'));
        });
        $router->map('f3', new Route('/f3', 'Controller@action'));

        $router->match('/f1');
        $this->assertEquals(1, $count);

        $router->match('/f2');
        $this->assertEquals(2, $count);

        $router->match('/f3');
        $this->assertEquals(2, $count);
    }

    /**
     * @expectedException \Exception
     */
    public function testFilterCanThrowException() {
        $this->object->filterCallback('test', function() use (&$count) {
            throw new \Exception('Filter error!');
        });

        $this->object->map('root', (new Route('/', 'Controller@action'))->addFilter('test'));

        $this->object->match('/');
    }

    public function testGroupPrefixing() {
        $this->object->group(function(Router $router, Group $group) {
            $group->setPrefix('/pre/');

            $router->map('group1', new Route('/group-1', 'Controller@action'));
            $router->map('group2', new Route('/group-2', 'Controller@action'));
        });

        $this->object->map('solo', new Route('/solo', 'Controller@action'));

        $routes = $this->object->getRoutes();

        $this->assertEquals('/', $routes['root']->getPath());
        $this->assertEquals('/pre/group-1', $routes['group1']->getPath());
        $this->assertEquals('/pre/group-2', $routes['group2']->getPath());
        $this->assertEquals('/solo', $routes['solo']->getPath());
    }

    public function testGroupSuffixing() {
        $this->object->group(function(Router $router, Group $group) {
            $group->setSuffix('/post/');

            $router->map('group1', new Route('/group-1', 'Controller@action'));
            $router->map('group2', new Route('/group-2', 'Controller@action'));
        });

        $this->object->map('solo', new Route('/solo', 'Controller@action'));

        $routes = $this->object->getRoutes();

        $this->assertEquals('/', $routes['root']->getPath());
        $this->assertEquals('/group-1/post', $routes['group1']->getPath());
        $this->assertEquals('/group-2/post', $routes['group2']->getPath());
        $this->assertEquals('/solo', $routes['solo']->getPath());
    }

    public function testGroupSecure() {
        $this->object->group(function(Router $router, Group $group) {
            $group->setSecure(true);

            $router->map('group1', new Route('/group-1', 'Controller@action'));
            $router->map('group2', new Route('/group-2', 'Controller@action'));
        });

        $this->object->map('solo', new Route('/solo', 'Controller@action'));

        $routes = $this->object->getRoutes();

        $this->assertEquals(false, $routes['root']->getSecure());
        $this->assertEquals(true, $routes['group1']->getSecure());
        $this->assertEquals(true, $routes['group2']->getSecure());
        $this->assertEquals(false, $routes['solo']->getSecure());
    }

    public function testGroupPatterns() {
        $this->object->group(function(Router $router, Group $group) {
            $group->setPrefix('<token>');
            $group->addPattern('token', '([abcd]+)');

            $router->map('group1', new Route('/group-1', 'Controller@action'));
            $router->map('group2', (new Route('/group-2', 'Controller@action'))->addPattern('foo', '(bar|baz)'));
        });

        $this->object->map('solo', new Route('/solo', 'Controller@action'));

        $routes = $this->object->getRoutes();

        $this->assertEquals('/', $routes['root']->getPath());
        $this->assertEquals('/<token>/group-1', $routes['group1']->getPath());
        $this->assertEquals('/<token>/group-2', $routes['group2']->getPath());
        $this->assertEquals('/solo', $routes['solo']->getPath());

        $this->assertEquals(Map {}, $routes['root']->getPatterns());
        $this->assertEquals(Map {'token' => '([abcd]+)'}, $routes['group1']->getPatterns());
        $this->assertEquals(Map {'foo' => '(bar|baz)', 'token' => '([abcd]+)'}, $routes['group2']->getPatterns());
        $this->assertEquals(Map {}, $routes['solo']->getPatterns());
    }

    public function testGroupConditions() {
        $cond1 = function() {};
        $cond2 = function() {};

        $this->object->group(function(Router $router, Group $group) use ($cond1, $cond2) {
            $group->setConditions(Vector {$cond1, $cond2});

            $router->map('group1', new Route('/group-1', 'Controller@action'));
            $router->map('group2', new Route('/group-2', 'Controller@action'));
        });

        $this->object->map('solo', new Route('/solo', 'Controller@action'));

        $routes = $this->object->getRoutes();

        $this->assertEquals(Vector {}, $routes['root']->getConditions());
        $this->assertEquals(Vector {$cond1, $cond2}, $routes['group1']->getConditions());
        $this->assertEquals(Vector {$cond1, $cond2}, $routes['group2']->getConditions());
        $this->assertEquals(Vector {}, $routes['solo']->getConditions());
    }

    public function testGroupNesting() {
        $this->object->group(function(Router $router, Group $group1) {
            $group1->setPrefix('/pre/');

            $router->map('group1', new Route('/group-1', 'Controller@action'));

            $router->group(function(Router $router, Group $group2) {
                $group2->setSuffix('/post');

                $router->map('group2', new Route('/group-2', 'Controller@action'));
            });
        });

        $this->object->map('solo', new Route('/solo', 'Controller@action'));

        $routes = $this->object->getRoutes();

        $this->assertEquals('/', $routes['root']->getPath());
        $this->assertEquals('/pre/group-1', $routes['group1']->getPath());
        $this->assertEquals('/pre/group-2/post', $routes['group2']->getPath());
        $this->assertEquals('/solo', $routes['solo']->getPath());
    }

    public function testGroupNestingInherits() {
        $this->object->group(function(Router $router, Group $group1) {
            $group1->setFilters(Vector {'foo'})->setMethods(Vector {'get'});

            $router->map('group1', new Route('/group-1', 'Controller@action'));

            $router->group(function(Router $router, Group $group2) {
                $group2->setFilters(Vector {'bar'});

                $router->map('group2', new Route('/group-2', 'Controller@action'));

                $router->group(function(Router $router, Group $group3) {
                    $group3->setMethods(Vector {'post'});

                    $router->map('group3', new Route('/group-3', 'Controller@action'));
                });
            });
        });

        $routes = $this->object->getRoutes();

        $this->assertEquals(Vector {'foo'}, $routes['group1']->getFilters());
        $this->assertEquals(Vector {'foo', 'bar'}, $routes['group2']->getFilters());
        $this->assertEquals(Vector {'foo', 'bar'}, $routes['group3']->getFilters());

        $this->assertEquals(Vector {'get'}, $routes['group1']->getMethods());
        $this->assertEquals(Vector {'get'}, $routes['group2']->getMethods());
        $this->assertEquals(Vector {'get', 'post'}, $routes['group3']->getMethods());
    }

    public function testHttpMapping() {
        $this->object->map('url1', new Route('/url', 'Controller@action'));
        $this->object->get('url2', new Route('/url', 'Controller@action'));
        $this->object->post('url3', new Route('/url', 'Controller@action'));
        $this->object->put('url4', new Route('/url', 'Controller@action'));
        $this->object->delete('url5', new Route('/url', 'Controller@action'));
        $this->object->head('url6', new Route('/url', 'Controller@action'));
        $this->object->options('url7', new Route('/url', 'Controller@action'));
        $this->object->http('url8', Vector {'get', 'post'}, new Route('/url', 'Controller@action'));

        $routes = $this->object->getRoutes();

        $this->assertEquals(Vector {}, $routes['url1']->getMethods());
        $this->assertEquals(Vector {'get'}, $routes['url2']->getMethods());
        $this->assertEquals(Vector {'post'}, $routes['url3']->getMethods());
        $this->assertEquals(Vector {'put'}, $routes['url4']->getMethods());
        $this->assertEquals(Vector {'delete'}, $routes['url5']->getMethods());
        $this->assertEquals(Vector {'head'}, $routes['url6']->getMethods());
        $this->assertEquals(Vector {'options'}, $routes['url7']->getMethods());
        $this->assertEquals(Vector {'get', 'post'}, $routes['url8']->getMethods());
    }

    public function testLoopMatch() {
        $route = $this->object->match('/');
        $this->assertEquals('/', $route->getPath());
        $this->assertEquals($route, $this->object->current());

        $route = $this->object->match('/users');
        $this->assertEquals('/{module}', $route->getPath());

        $route = $this->object->match('/users/profile');
        $this->assertEquals('/{module}/{controller}', $route->getPath());

        $route = $this->object->match('/users/profile/view');
        $this->assertEquals('/{module}/{controller}/{action}', $route->getPath());

        $route = $this->object->match('/users/profile/view.json');
        $this->assertEquals('/{module}/{controller}/{action}.{ext}', $route->getPath());
    }

    /**
     * @expectedException \Titon\Route\Exception\NoMatchException
     */
    public function testLoopMatchNoMatch() {
        $this->object->match('/path~tilde');
    }

    public function testParseAction() {
        $this->assertEquals(shape(
            'class' => 'Controller',
            'action' => 'action'
        ), Router::parseAction('Controller@action'));

        $this->assertEquals(shape(
            'class' => 'Controller',
            'action' => 'foo'
        ), Router::parseAction('Controller@foo'));

        $this->assertEquals(shape(
            'class' => 'Module\Controller',
            'action' => 'index'
        ), Router::parseAction('Module\Controller@index'));

        $this->assertEquals(shape(
            'class' => 'Module\Controller_With_Underscores',
            'action' => 'index'
        ), Router::parseAction('Module\Controller_With_Underscores@index'));

        $this->assertEquals(shape(
            'class' => 'Module\Controller_With_Numbers123',
            'action' => 'index'
        ), Router::parseAction('Module\Controller_With_Numbers123@index'));

        $this->assertEquals(shape(
            'class' => 'Module\Controller',
            'action' => 'action'
        ), Router::parseAction('Module\Controller@action'));

        $this->assertEquals(shape(
            'class' => 'Module\Controller',
            'action' => 'multiWordAction'
        ), Router::parseAction('Module\Controller@multiWordAction'));

        $this->assertEquals(shape(
            'class' => 'Module\Controller',
            'action' => 'action_with_underscores'
        ), Router::parseAction('Module\Controller@action_with_underscores'));
    }

    /**
     * @expectedException \Titon\Route\Exception\InvalidRouteActionException
     */
    public function testParseActionInvalidRoute() {
        Router::parseAction('Broken+Route');
    }

    public function testPrgMapping() {
        $this->object->prg('prg', (new Route('/foo', 'Controller@action'))->addFilter('auth'));

        $routes = $this->object->getRoutes();

        // Keys
        $this->assertFalse(isset($routes['prg']));
        $this->assertTrue(isset($routes['prg.get']));
        $this->assertTrue(isset($routes['prg.post']));

        // Paths
        $this->assertEquals('/foo', $routes['prg.get']->getPath());
        $this->assertEquals('/foo', $routes['prg.post']->getPath());

        // Action
        $this->assertEquals(shape('class' => 'Controller', 'action' => 'getAction'), $routes['prg.get']->getAction());
        $this->assertEquals(shape('class' => 'Controller', 'action' => 'postAction'), $routes['prg.post']->getAction());

        // Method
        $this->assertEquals(Vector {'get'}, $routes['prg.get']->getMethods());
        $this->assertEquals(Vector {'post'}, $routes['prg.post']->getMethods());

        // Filters should be cloned also
        $this->assertEquals(Vector {'auth'}, $routes['prg.get']->getFilters());
        $this->assertEquals(Vector {'auth'}, $routes['prg.post']->getFilters());
    }

    public function testResourceMap() {
        $this->assertEquals(Map {
            'list' => 'index',
            'create' => 'create',
            'read' => 'read',
            'update' => 'update',
            'delete' => 'delete'
        }, $this->object->getResourceMap());

        $this->object->setResourceMap(Map {
            'create' => 'add',
            'read' => 'view',
            'update' => 'edit',
            'delete' => 'remove'
        });

        $this->assertEquals(Map {
            'list' => 'index',
            'create' => 'add',
            'read' => 'view',
            'update' => 'edit',
            'delete' => 'remove'
        }, $this->object->getResourceMap());
    }

    public function testResourceMapping() {
        $this->object->resource('rest', new Route('/rest', 'Api\Rest@action'));

        $routes = $this->object->getRoutes();

        // Keys
        $this->assertFalse(isset($routes['rest']));
        $this->assertTrue(isset($routes['rest.list']));
        $this->assertTrue(isset($routes['rest.create']));
        $this->assertTrue(isset($routes['rest.read']));
        $this->assertTrue(isset($routes['rest.update']));
        $this->assertTrue(isset($routes['rest.delete']));

        // Paths
        $this->assertEquals('/rest', $routes['rest.list']->getPath());
        $this->assertEquals('/rest', $routes['rest.create']->getPath());
        $this->assertEquals('/rest/{id}', $routes['rest.read']->getPath());
        $this->assertEquals('/rest/{id}', $routes['rest.update']->getPath());
        $this->assertEquals('/rest/{id}', $routes['rest.delete']->getPath());

        // Action
        $this->assertEquals(shape('class' => 'Api\Rest', 'action' => 'index'), $routes['rest.list']->getAction());
        $this->assertEquals(shape('class' => 'Api\Rest', 'action' => 'create'), $routes['rest.create']->getAction());
        $this->assertEquals(shape('class' => 'Api\Rest', 'action' => 'read'), $routes['rest.read']->getAction());
        $this->assertEquals(shape('class' => 'Api\Rest', 'action' => 'update'), $routes['rest.update']->getAction());
        $this->assertEquals(shape('class' => 'Api\Rest', 'action' => 'delete'), $routes['rest.delete']->getAction());

        // Method
        $this->assertEquals(Vector {'get'}, $routes['rest.list']->getMethods());
        $this->assertEquals(Vector {'post'}, $routes['rest.create']->getMethods());
        $this->assertEquals(Vector {'get'}, $routes['rest.read']->getMethods());
        $this->assertEquals(Vector {'put', 'post'}, $routes['rest.update']->getMethods());
        $this->assertEquals(Vector {'delete', 'post'}, $routes['rest.delete']->getMethods());
    }

    public function TestRouteStubs() {
        $route = new Route('/', 'Controller@action');
        $router = new Router();
        $router->map('key', $route);

        $this->assertEquals($route, $router->getRoute('key'));
        $this->assertEquals(Map {'key' => $route}, $router->getRoutes());
    }

    /**
     * @expectedException \Titon\Route\Exception\MissingRouteException
     */
    public function TestRouteStubsMissingKey() {
        $this->object->getRoute('fakeKey');
    }

    public function testSegments() {
        $_SERVER['DOCUMENT_ROOT'] = '';
        $_SERVER['HTTP_HOST'] = 'localhost';
        $_SERVER['SCRIPT_FILENAME'] = '/index.php';
        $_SERVER['REQUEST_URI'] = '/';
        Server::initialize($_SERVER);

        $router = new Router();
        $this->assertEquals('/', $router->base());
        $this->assertEquals('http://localhost/', Depository::getInstance()->make('Titon\Route\UrlBuilder', $router)->url());
        $this->assertEquals('/', $router->getSegment('path'));
        $this->assertInstanceOf('HH\Map', $router->getSegment('query'));
        $this->assertEquals(Map {
            'path' => '/',
            'scheme' => 'http',
            'query' => Map {},
            'host' => 'localhost',
            'port' => 80
        }, $router->getSegments());

        // module, controller
        $_SERVER['HTTP_HOST'] = 'domain.com';
        $_SERVER['SCRIPT_FILENAME'] = '/index.php';
        $_SERVER['REQUEST_URI'] = '/module/index';
        Server::initialize($_SERVER);

        $router = new Router();
        $this->assertEquals('/', $router->base());
        $this->assertEquals('http://domain.com/module/index', Depository::getInstance()->make('Titon\Route\UrlBuilder', $router)->url());
        $this->assertEquals('/module/index', $router->getSegment('path'));
        $this->assertInstanceOf('HH\Map', $router->getSegment('query'));
        $this->assertEquals(Map {
            'path' => '/module/index',
            'scheme' => 'http',
            'query' => Map {},
            'host' => 'domain.com',
            'port' => 80
        }, $router->getSegments());

        // module, controller, action, ext, base,
        $_SERVER['HTTP_HOST'] = 'sub.domain.com';
        $_SERVER['SCRIPT_FILENAME'] = '/root/dir/index.php';
        $_SERVER['REQUEST_URI'] = '/module/controller/action.html';
        Server::initialize($_SERVER);

        $router = new Router();
        $this->assertEquals('/root/dir', $router->base());
        $this->assertEquals('http://sub.domain.com/root/dir/module/controller/action.html', Depository::getInstance()->make('Titon\Route\UrlBuilder', $router)->url());
        $this->assertEquals('/module/controller/action.html', $router->getSegment('path'));
        $this->assertInstanceOf('HH\Map', $router->getSegment('query'));
        $this->assertEquals(Map {
            'path' => '/module/controller/action.html',
            'scheme' => 'http',
            'query' => Map {},
            'host' => 'sub.domain.com',
            'port' => 80
        }, $router->getSegments());

        // module, controller, action, ext, base, query, https
        $_SERVER['HTTP_HOST'] = 'subber.sub.domain.com';
        $_SERVER['SCRIPT_FILENAME'] = '/rooter/root/dir/index.php'; // query doesn't show up here
        $_SERVER['REQUEST_URI'] = '/module/controller/action.html?foo=bar&int=123';
        $_SERVER['HTTPS'] = 'on';
        Server::initialize($_SERVER);

        $_GET = ['foo' => 'bar', 'int' => 123];
        Get::initialize($_GET);

        $router = new Router();
        $this->assertEquals('/rooter/root/dir', $router->base());
        $this->assertEquals('https://subber.sub.domain.com/rooter/root/dir/module/controller/action.html?foo=bar&int=123', Depository::getInstance()->make('Titon\Route\UrlBuilder', $router)->url());
        $this->assertEquals('/module/controller/action.html', $router->getSegment('path'));
        $this->assertInstanceOf('HH\Map', $router->getSegment('query'));
        $this->assertEquals(Map {
            'path' => '/module/controller/action.html',
            'scheme' => 'https',
            'query' => Map {'foo' => 'bar', 'int' => 123},
            'host' => 'subber.sub.domain.com',
            'port' => 80
        }, $router->getSegments());

        // module, controller, action, ext, base, query, https, args
        $_SERVER['HTTP_HOST'] = 'subbest.subber.sub.domain.com';
        $_SERVER['SCRIPT_FILENAME'] = '/base/rooter/root/dir/index.php'; // query doesn't show up here
        $_SERVER['REQUEST_URI'] = '/module/controller/action.html/123/abc?foo=bar&int=123';
        $_SERVER['HTTPS'] = 'on';
        Server::initialize($_SERVER);

        $_GET = ['foo' => 'bar', 'int' => 123];
        Get::initialize($_GET);

        $router = new Router();
        $this->assertEquals('/base/rooter/root/dir', $router->base());
        $this->assertEquals('https://subbest.subber.sub.domain.com/base/rooter/root/dir/module/controller/action.html/123/abc?foo=bar&int=123', Depository::getInstance()->make('Titon\Route\UrlBuilder', $router)->url());
        $this->assertEquals('/module/controller/action.html/123/abc', $router->getSegment('path'));
        $this->assertInstanceOf('HH\Map', $router->getSegment('query'));
        $this->assertEquals(Map {
            'path' => '/module/controller/action.html/123/abc',
            'scheme' => 'https',
            'query' => Map {'foo' => 'bar', 'int' => 123},
            'host' => 'subbest.subber.sub.domain.com',
            'port' => 80
        }, $router->getSegments());
    }

    /**
     * @expectedException \Titon\Route\Exception\MissingSegmentException
     */
    public function testSegmentsMissingKey() {
        $this->object->getSegment('fakeKey');
    }

    public function testWireClassMapping() {
        $this->object->wire('Titon\Test\Stub\Route\RouteAnnotatedStub');

        $routes = $this->object->getRoutes();

        // Keys
        $this->assertFalse(isset($routes['parent']));
        $this->assertTrue(isset($routes['parent.list']));
        $this->assertTrue(isset($routes['parent.create']));
        $this->assertTrue(isset($routes['parent.read']));
        $this->assertTrue(isset($routes['parent.update']));
        $this->assertTrue(isset($routes['parent.delete']));

        // Paths
        $this->assertEquals('/controller', $routes['parent.list']->getPath());
        $this->assertEquals('/controller', $routes['parent.create']->getPath());
        $this->assertEquals('/controller/{id}', $routes['parent.read']->getPath());
        $this->assertEquals('/controller/{id}', $routes['parent.update']->getPath());
        $this->assertEquals('/controller/{id}', $routes['parent.delete']->getPath());

        // Action
        $this->assertEquals(shape('class' => 'Titon\Test\Stub\Route\RouteAnnotatedStub', 'action' => 'index'), $routes['parent.list']->getAction());
        $this->assertEquals(shape('class' => 'Titon\Test\Stub\Route\RouteAnnotatedStub', 'action' => 'create'), $routes['parent.create']->getAction());
        $this->assertEquals(shape('class' => 'Titon\Test\Stub\Route\RouteAnnotatedStub', 'action' => 'read'), $routes['parent.read']->getAction());
        $this->assertEquals(shape('class' => 'Titon\Test\Stub\Route\RouteAnnotatedStub', 'action' => 'update'), $routes['parent.update']->getAction());
        $this->assertEquals(shape('class' => 'Titon\Test\Stub\Route\RouteAnnotatedStub', 'action' => 'delete'), $routes['parent.delete']->getAction());

        // Method
        $this->assertEquals(Vector {'get'}, $routes['parent.list']->getMethods());
        $this->assertEquals(Vector {'post'}, $routes['parent.create']->getMethods());
        $this->assertEquals(Vector {'get'}, $routes['parent.read']->getMethods());
        $this->assertEquals(Vector {'put', 'post'}, $routes['parent.update']->getMethods());
        $this->assertEquals(Vector {'delete', 'post'}, $routes['parent.delete']->getMethods());
    }

    public function testWireMethodMapping() {
        $this->object->wire('Titon\Test\Stub\Route\RouteAnnotatedStub');

        $routes = $this->object->getRoutes();

        // Keys
        $this->assertTrue(isset($routes['foo']));
        $this->assertTrue(isset($routes['bar']));
        $this->assertTrue(isset($routes['baz']));
        $this->assertTrue(isset($routes['qux']));

        // Paths
        $this->assertEquals('/foo', $routes['foo']->getPath());
        $this->assertEquals('/bar', $routes['bar']->getPath());
        $this->assertEquals('/baz', $routes['baz']->getPath());
        $this->assertEquals('/qux', $routes['qux']->getPath());

        // Action
        $this->assertEquals(shape('class' => 'Titon\Test\Stub\Route\RouteAnnotatedStub', 'action' => 'foo'), $routes['foo']->getAction());
        $this->assertEquals(shape('class' => 'Titon\Test\Stub\Route\RouteAnnotatedStub', 'action' => 'bar'), $routes['bar']->getAction());
        $this->assertEquals(shape('class' => 'Titon\Test\Stub\Route\RouteAnnotatedStub', 'action' => 'baz'), $routes['baz']->getAction());
        $this->assertEquals(shape('class' => 'Titon\Test\Stub\Route\RouteAnnotatedStub', 'action' => 'qux'), $routes['qux']->getAction());

        // Method
        $this->assertEquals(Vector {}, $routes['foo']->getMethods());
        $this->assertEquals(Vector {'post'}, $routes['bar']->getMethods());
        $this->assertEquals(Vector {'get'}, $routes['baz']->getMethods());
        $this->assertEquals(Vector {'put', 'post'}, $routes['qux']->getMethods());

        // Filter
        $this->assertEquals(Vector {}, $routes['foo']->getFilters());
        $this->assertEquals(Vector {}, $routes['bar']->getFilters());
        $this->assertEquals(Vector {'auth', 'guest'}, $routes['baz']->getFilters());
        $this->assertEquals(Vector {}, $routes['qux']->getFilters());

        // Pattern
        $this->assertEquals(Map {}, $routes['foo']->getPatterns());
        $this->assertEquals(Map {}, $routes['bar']->getPatterns());
        $this->assertEquals(Map {}, $routes['baz']->getPatterns());
        $this->assertEquals(Map {'id' => '[1-8]+'}, $routes['qux']->getPatterns());
    }

}
