<?hh
namespace Titon\Route;

use Titon\Test\Stub\Route\TestRouteStub;
use Titon\Test\TestCase;
use Titon\Utility\State\Server;

class RouteTest extends TestCase {

    public function testAppendPrepend(): void {
        $route = new Route('/', 'Controller@action');

        $this->assertEquals('/', $route->getPath());

        $route->prepend('/prepend/');
        $this->assertEquals('/prepend', $route->getPath());

        $route->prepend('before');
        $this->assertEquals('/before/prepend', $route->getPath());

        $route->prepend('{prefix}//');
        $this->assertEquals('/{prefix}/before/prepend', $route->getPath());

        $route->append('/append/');
        $this->assertEquals('/{prefix}/before/prepend/append', $route->getPath());

        $route->append('after//');
        $this->assertEquals('/{prefix}/before/prepend/append/after', $route->getPath());

        $route->append('//{suffix}');
        $this->assertEquals('/{prefix}/before/prepend/append/after/{suffix}', $route->getPath());
    }

    public function testConditions(): void {
        $route = new Route('/', 'Controller@action');

        $this->assertEquals(Vector {}, $route->getConditions());

        $cond1 = ($route) ==> { return true; };
        $cond2 = ($route) ==> { return true; };
        $cond3 = ($route) ==> { return true; };

        $route->addCondition($cond1);
        $route->addConditions(Vector {$cond3, $cond2});

        $this->assertEquals(Vector {$cond1, $cond3, $cond2}, $route->getConditions());

        $route->setConditions(Vector {$cond2});

        $this->assertEquals(Vector {$cond2}, $route->getConditions());
    }

    public function testCompile(): void {
        $moduleControllerActionExt = new Route('/{module}/{controller}/{action}.{ext}', 'Controller@action');
        $moduleControllerAction = new Route('/{module}/{controller}/{action}', 'Controller@action');
        $moduleController = new Route('/{module}/{controller}', 'Controller@action');
        $module = new Route('/{module}', 'Controller@action');
        $root = (new Route('/', 'Controller@action'))->setStatic(true);

        $this->assertEquals('\/([a-z0-9\_\-\.]+)\/([a-z0-9\_\-\.]+)\/([a-z0-9\_\-\.]+)\.([a-z0-9\_\-\.]+)\/?', $moduleControllerActionExt->compile());
        $this->assertEquals('\/([a-z0-9\_\-\.]+)\/([a-z0-9\_\-\.]+)\/([a-z0-9\_\-\.]+)\/?', $moduleControllerAction->compile());
        $this->assertEquals('\/([a-z0-9\_\-\.]+)\/([a-z0-9\_\-\.]+)\/?', $moduleController->compile());
        $this->assertEquals('\/([a-z0-9\_\-\.]+)\/?', $module->compile());
        $this->assertEquals('\/', $root->compile());

        $multi = new Route('{alpha}/[numeric]/(wildcard)/', 'Controller@action');

        $patterns = (new Route('<alnum>/<locale>', 'Controller@action'))->setPatterns(Map {
            'alnum' => Route::ALNUM,
            'locale' => '([a-z]{2}(?:-[a-z]{2})?)'
        });

        $allTypes = (new Route('/<locale>/{alpha}/(wildcard)/[numeric]/{alnum}', 'Controller@action'))->setPatterns(Map {
            'alnum' => Route::ALNUM,
            'locale' => '([a-z]{2}(?:-[a-z]{2})?)'
        });

        $this->assertEquals('\/([a-z0-9\_\-\.]+)\/([0-9\.]+)\/([^\/]+)\/?', $multi->compile());
        $this->assertEquals('\/([a-z0-9\_\-\.]+)\/([a-z]{2}(?:-[a-z]{2})?)\/?', $patterns->compile());
        $this->assertEquals('\/([a-z]{2}(?:-[a-z]{2})?)\/([a-z0-9\_\-\.]+)\/([^\/]+)\/([0-9\.]+)\/([a-z0-9\_\-\.]+)\/?', $allTypes->compile());
    }

    /**
     * @expectedException \Titon\Route\Exception\MissingPatternException
     */
    public function testCompileMissingPattern(): void {
        $noPattern = new Route('<missing>', 'Controller@action');
        $noPattern->compile();
    }

    public function testCompileOptionalToken(): void {
        $route = new Route('/users/[id]', 'Controller@action');

        $this->assertEquals('\/users\/([0-9\.]+)\/?', $route->compile());
        $this->assertTrue($route->isMatch('/users/1'));
        $this->assertFalse($route->isMatch('/users'));

        $route = new Route('/users/[id?]', 'Controller@action');

        $this->assertEquals('\/users(?:\/([0-9\.]+))?\/?', $route->compile());
        $this->assertTrue($route->isMatch('/users/1'));
        $this->assertTrue($route->isMatch('/users/'));
        $this->assertTrue($route->isMatch('/users'));
    }

    public function testDispatch(): void {
        $route = new Route('/{a}/{b}', 'Titon\Test\Stub\Route\DispatchRouteStub@noOptional');
        $route->isMatch('/foo/bar');

        $this->assertEquals('foobar', $route->dispatch());
    }

    public function testDispatchOptional(): void {
        $route = new Route('/{a}/{b?}', 'Titon\Test\Stub\Route\DispatchRouteStub@withOptional');
        $route->isMatch('/foo');

        $this->assertEquals('foobaz', $route->dispatch());
    }

    public function testDispatchTypeHints(): void {
        $route = new Route('/{a}/[b]/(c)', 'Titon\Test\Stub\Route\DispatchRouteStub@typeHints');
        $route->isMatch('/foo/123/bar_456');

        $this->assertEquals('foo123bar_456', $route->dispatch());
    }

    /**
     * @expectedException \Titon\Route\Exception\NoMatchException
     */
    public function testDispatchNoMatch(): void {
        $route = new Route('/{a}/{b}', 'Titon\Test\Stub\Route\DispatchRouteStub@noOptional');
        $route->dispatch();
    }

    public function testFilters(): void {
        $route = new Route('/', 'Controller@action');

        $this->assertEquals(Vector {}, $route->getFilters());

        $route->addFilter('foo');
        $route->addFilters(Vector {'bar', 'foo'});

        $this->assertEquals(Vector {'foo', 'bar'}, $route->getFilters());

        $route->setFilters(Vector {'baz'});

        $this->assertEquals(Vector {'baz'}, $route->getFilters());
    }

    public function testGetAction(): void {
        $route = new Route('/', 'Controller@action');

        $this->assertEquals(shape('class' => 'Controller', 'action' => 'action'), $route->getAction());
    }

    public function testGetPath(): void {
        $route = new Route('/users/profile/[id]', 'Controller@action');

        $this->assertEquals('/users/profile/[id]', $route->getPath());
    }

    public function testIsMethod(): void {
        $noMethod = new Route('/', 'Controller@action');
        $singleMethod = (new Route('/', 'Controller@action'))->addMethod('POST');
        $multiMethod = (new Route('/', 'Controller@action'))->setMethods(Vector {'post', 'put'});

        $_SERVER['REQUEST_METHOD'] = 'GET';
        Server::initialize($_SERVER);

        $this->assertTrue($noMethod->isMethod());
        $this->assertFalse($singleMethod->isMethod());
        $this->assertFalse($multiMethod->isMethod());

        $_SERVER['REQUEST_METHOD'] = 'POST';
        Server::initialize($_SERVER);

        $this->assertTrue($noMethod->isMethod());
        $this->assertTrue($singleMethod->isMethod());
        $this->assertTrue($multiMethod->isMethod());

        $_SERVER['REQUEST_METHOD'] = 'PUT';
        Server::initialize($_SERVER);

        $this->assertTrue($noMethod->isMethod());
        $this->assertFalse($singleMethod->isMethod());
        $this->assertTrue($multiMethod->isMethod());
    }

    public function testIsSecure(): void {
        $unsecureRoute = (new Route('/', 'Controller@action'))->setSecure(false);
        $secureRoute = (new Route('/', 'Controller@action'))->setSecure(true);

        $this->assertTrue($unsecureRoute->isSecure());
        $this->assertFalse($secureRoute->isSecure());

        $_SERVER['HTTPS'] = 'on';
        Server::initialize($_SERVER);

        $this->assertTrue($unsecureRoute->isSecure());
        $this->assertTrue($secureRoute->isSecure());
    }

    public function testIsStatic(): void {
        $route = (new Route('/', 'Controller@action'))->setStatic(false);
        $staticRoute = (new Route('/', 'Controller@action'))->setStatic(true);
        $tokenRoute = new Route('/{module}', 'Controller@action');

        $this->assertFalse($route->isStatic());
        $this->assertTrue($staticRoute->isStatic());
        $this->assertFalse($tokenRoute->isStatic());
    }

    public function testIsMatch(): void {
        $route = new Route('/{module}/{controller}/{action}.{ext}', 'Controller@action');

        $this->assertFalse($route->isMatch('/users'));
        $this->assertFalse($route->isMatch('/users/profile'));
        $this->assertFalse($route->isMatch('/users/profile/activity'));
        $this->assertTrue($route->isMatch('/users/profile/activity.json'));
    }

    public function testIsMatchAlphaPattern(): void {
        $route = (new Route('/<alpha>', 'Controller@action'))->addPattern('alpha', Route::ALPHA);

        $this->assertTrue($route->isMatch('/FOO'));
        $this->assertTrue($route->isMatch('/foo'));
        $this->assertTrue($route->isMatch('/foo-bar'));
        $this->assertTrue($route->isMatch('/foo_bar'));
        $this->assertTrue($route->isMatch('/foo.bar'));
        $this->assertFalse($route->isMatch('/foo123'));
    }

    public function testIsMatchAlnumPattern(): void {
        $route = new Route('/{alpha}', 'Controller@action');

        $this->assertTrue($route->isMatch('/FOO'));
        $this->assertTrue($route->isMatch('/foo'));
        $this->assertTrue($route->isMatch('/foo-bar'));
        $this->assertTrue($route->isMatch('/foo_bar'));
        $this->assertTrue($route->isMatch('/foo.bar'));
        $this->assertTrue($route->isMatch('/foo123'));
        $this->assertTrue($route->isMatch('/foo-123'));
        $this->assertFalse($route->isMatch('/foo+123'));
    }

    public function testIsMatchNumericPattern(): void {
        $route = new Route('/[numeric]', 'Controller@action');

        $this->assertTrue($route->isMatch('/123'));
        $this->assertTrue($route->isMatch('/12.3'));
        $this->assertFalse($route->isMatch('/12-3'));
        $this->assertFalse($route->isMatch('/123foo'));
        $this->assertFalse($route->isMatch('/foo'));
    }

    public function testIsMatchWildcardPattern(): void {
        $route = new Route('/(wild)', 'Controller@action');

        $this->assertTrue($route->isMatch('/FOO'));
        $this->assertTrue($route->isMatch('/foo'));
        $this->assertTrue($route->isMatch('/foo-bar'));
        $this->assertTrue($route->isMatch('/foo_bar'));
        $this->assertTrue($route->isMatch('/foo+bar'));
        $this->assertTrue($route->isMatch('/foo.bar'));
        $this->assertTrue($route->isMatch('/foo123'));
        $this->assertTrue($route->isMatch('/foo-123'));
        $this->assertTrue($route->isMatch('/foo.123'));
    }

    public function testIsMatchLocalePattern(): void {
        $route = (new Route('/<locale>', 'Controller@action'))->addPattern('locale', Route::LOCALE);

        $this->assertTrue($route->isMatch('/en'));
        $this->assertTrue($route->isMatch('/en-us'));
        $this->assertFalse($route->isMatch('/en_us'));
        $this->assertFalse($route->isMatch('/eng'));
    }

    public function testIsMatchMethod(): void {
        $route = new Route('/{module}', 'Controller@action');

        $_SERVER['REQUEST_METHOD'] = 'GET';
        Server::initialize($_SERVER);

        $this->assertTrue($route->isMatch('/foo'));

        $route->addMethod('get');

        $this->assertTrue($route->isMatch('/foo'));

        $route->setMethods(Vector {'post'});

        $this->assertFalse($route->isMatch('/foo'));
    }

    public function testIsMatchSecure(): void {
        $route = new Route('/{module}', 'Controller@action');

        $_SERVER['HTTPS'] = 'off';
        Server::initialize($_SERVER);

        $this->assertTrue($route->isMatch('/foo'));

        $route->setSecure(true);

        $this->assertFalse($route->isMatch('/foo'));

        $_SERVER['HTTPS'] = 'on';
        Server::initialize($_SERVER);

        $this->assertTrue($route->isMatch('/foo'));
    }

    public function testIsMatchDirectEqualPath(): void {
        $route = new Route('/foo', 'Controller@action');

        $this->assertTrue($route->isMatch('/foo'));
    }

    public function testIsMatchTrailingSlash(): void {
        $route = new Route('/{module}', 'Controller@action');

        $this->assertTrue($route->isMatch('/users'));
        $this->assertTrue($route->isMatch('/users/'));
        $this->assertFalse($route->isMatch('/users//'));
    }

    public function testIsMatchMultipleOptionals(): void {
        $route = new Route('/{year}/{month?}/{day?}', 'Controller@action');

        $this->assertTrue($route->isMatch('/2014/01/01'));
        $this->assertTrue($route->isMatch('/2014/01'));
        $this->assertTrue($route->isMatch('/2014'));
    }

    public function testIsMatchConditions(): void {
        $route = new Route('/{module}', 'Controller@action');
        $route->addCondition(($route) ==> (strpos($_SERVER['HTTP_ACCEPT'], 'json') !== false));

        $_SERVER['HTTP_ACCEPT'] = 'text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8';

        $this->assertFalse($route->isMatch('/users'));

        $_SERVER['HTTP_ACCEPT'] = 'application/json;q=0.9';

        $this->assertTrue($route->isMatch('/users'));
    }

    public function testMethods(): void {
        $route = new Route('/', 'Controller@action');

        $this->assertEquals(Vector {}, $route->getMethods());

        $route->addMethod('get');
        $route->addMethods(Vector {'post', 'get'});

        $this->assertEquals(Vector {'get', 'post'}, $route->getMethods());

        $route->setMethods(Vector {'put'});

        $this->assertEquals(Vector {'put'}, $route->getMethods());
    }

    public function testParams(): void {
        $route = new Route('/{module}/{controller}/{action}.{ext}', 'Controller@action');
        $route->isMatch('/users/profile/activity.json');

        $this->assertEquals('/users/profile/activity.json', $route->url());

        $this->assertEquals(Map {
            'module' => 'users',
            'controller' => 'profile',
            'action' => 'activity',
            'ext' => 'json'
        }, $route->getParams());

        $this->assertEquals('users', $route->getParam('module'));
        $this->assertEquals(null, $route->getParam('hash'));
    }

    public function testParamsOptional(): void {
        $route = new TestRouteStub('/blog/[year]/[month]/[day?]', 'Module\Controller@action');
        $route->isMatch('/blog/2014/02');

        $this->assertEquals('/blog/2014/02', $route->url());

        $this->assertEquals(Map {
            'year' => '2014',
            'month' => '02',
            'day' => ''
        }, $route->getParams());

        $route->isMatch('/blog/2014/02/05');

        $this->assertEquals('/blog/2014/02/05', $route->url());

        $this->assertEquals(Map {
            'year' => '2014',
            'month' => '02',
            'day' => '05'
        }, $route->getParams());
    }

    public function testPatterns(): void {
        $route = new Route('/', 'Controller@action');

        $this->assertEquals(Map {}, $route->getPatterns());

        $route->addPattern('id', '[0-9]+');
        $route->addPatterns(Map {'key' => '[a-z]+', 'id' => '[1-8]+'});

        $this->assertEquals(Map {'id' => '[1-8]+', 'key' => '[a-z]+'}, $route->getPatterns());

        $route->setPatterns(Map {'key' => '[A-Z]+'});

        $this->assertEquals(Map {'key' => '[A-Z]+'}, $route->getPatterns());
    }

    public function testTokens(): void {
        $route = new Route('/{string}', 'Controller@action');
        $this->assertEquals('\/([a-z0-9\_\-\.]+)\/?', $route->compile());

        $route = new Route('/{string?}', 'Controller@action');
        $this->assertEquals('(?:\/([a-z0-9\_\-\.]+))?\/?', $route->compile());

        $route = new Route('/[int]', 'Controller@action');
        $this->assertEquals('\/([0-9\.]+)\/?', $route->compile());

        $route = new Route('/[int?]', 'Controller@action');
        $this->assertEquals('(?:\/([0-9\.]+))?\/?', $route->compile());

        $route = new Route('/(wildcard)', 'Controller@action');
        $this->assertEquals('\/([^\/]+)\/?', $route->compile());

        $route = new Route('/(wildcard?)', 'Controller@action');
        $this->assertEquals('(?:\/([^\/]+))?\/?', $route->compile());

        $route = (new Route('/<regex>', 'Controller@action'))->addPattern('regex', '[foo|bar]');
        $this->assertEquals('\/([foo|bar])\/?', $route->compile());

        $route = (new Route('/<regex?>', 'Controller@action'))->addPattern('regex', '[foo|bar]');
        $this->assertEquals('(?:\/([foo|bar]))?\/?', $route->compile());

        $route = new Route('/<regex:[foo|bar]>', 'Controller@action');
        $this->assertEquals('\/([foo|bar])\/?', $route->compile());

        $route = new Route('/<regex:[foo|bar]?>', 'Controller@action');
        $this->assertEquals('(?:\/([foo|bar]))?\/?', $route->compile());

        $route = new Route('/<regex:([a-z0-9\w\s\d\-]+)?>', 'Controller@action');
        $this->assertEquals('(?:\/([a-z0-9\w\s\d\-]+))?\/?', $route->compile());
        $this->assertEquals(Map {'regex' => '([a-z0-9\w\s\d\-]+)'}, $route->getPatterns());

        $route = new Route('/{string}/[int]', 'Controller@action');
        $this->assertEquals('\/([a-z0-9\_\-\.]+)\/([0-9\.]+)\/?', $route->compile());

        $route = new Route('/(wild)/{string}/[int?]', 'Controller@action');
        $this->assertEquals('\/([^\/]+)\/([a-z0-9\_\-\.]+)(?:\/([0-9\.]+))?\/?', $route->compile());

        $route = new Route('/(wild)/{string}/<regex:([a-z]\-[A-Z])>/[int?]', 'Controller@action');
        $this->assertEquals('\/([^\/]+)\/([a-z0-9\_\-\.]+)\/([a-z]\-[A-Z])(?:\/([0-9\.]+))?\/?', $route->compile());
    }

    public function testSerialize(): void {
        $route = new Route('/{module}/{controller}/{action}.{ext}', 'Controller@action');
        $route->addFilter('foo');
        $route->addMethod('post');
        $route->addPattern('locale', Route::LOCALE);
        $route->setSecure(true);
        $route->compile();

        $serialized = serialize($route);

        $this->assertEquals('C:17:"Titon\Route\Route":685:{K:6:"HH\Map":9:{s:6:"action";a:2:{s:5:"class";s:10:"Controller";s:6:"action";s:6:"action";}s:8:"compiled";s:79:"\/([a-z0-9\_\-\.]+)\/([a-z0-9\_\-\.]+)\/([a-z0-9\_\-\.]+)\.([a-z0-9\_\-\.]+)\/?";s:7:"filters";V:9:"HH\Vector":1:{s:3:"foo";}s:7:"methods";V:9:"HH\Vector":1:{s:4:"post";}s:8:"patterns";K:6:"HH\Map":1:{s:6:"locale";s:24:"([a-z]{2}(?:-[a-z]{2})?)";}s:4:"path";s:37:"/{module}/{controller}/{action}.{ext}";s:6:"secure";b:1;s:6:"static";b:0;s:6:"tokens";V:9:"HH\Vector":4:{a:2:{s:5:"token";s:6:"module";s:8:"optional";b:0;}a:2:{s:5:"token";s:10:"controller";s:8:"optional";b:0;}a:2:{s:5:"token";s:6:"action";s:8:"optional";b:0;}a:2:{s:5:"token";s:3:"ext";s:8:"optional";b:0;}}}}', $serialized);
        $this->assertEquals($route, unserialize($serialized));
    }

}
