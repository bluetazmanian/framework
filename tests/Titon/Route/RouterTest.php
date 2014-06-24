<?php
namespace Titon\Route;

use Titon\Common\Config;
use Titon\Test\TestCase;

/**
 * @property \Titon\Route\Router $object
 */
class RouterTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        $this->object = new Router();
        $this->object->map('action.ext', new Route('/{module}/{controller}/{action}.{ext}'));
        $this->object->map('action', new Route('/{module}/{controller}/{action}'));
        $this->object->map('controller', new Route('/{module}/{controller}'));
        $this->object->map('module', new Route('/{module}'));
        $this->object->map('root', new Route('/'));
        $this->object->initialize();
    }

    public function testBuild() {
        $this->assertEquals('/', $this->object->build());
        $this->assertEquals('/module', $this->object->build(['module' => 'module']));
        $this->assertEquals('/module/index/action', $this->object->build(['module' => 'module', 'action' => 'action']));
        $this->assertEquals('/module/controller/action', $this->object->build(['module' => 'module', 'controller' => 'controller', 'action' => 'action']));
        $this->assertEquals('/main/controller/action', $this->object->build(['controller' => 'controller', 'action' => 'action']));
        $this->assertEquals('/dash-ed/camelcase/under-score', $this->object->build(['module' => 'dash-ed', 'controller' => 'camelCase', 'action' => 'under_score']));
        $this->assertEquals('/s-p-a-c-e-s/1-2-3-numbers/punctuation', $this->object->build(['module' => 's p a c e s', 'controller' => '1 2 3 numbers', 'action' => 'punc"@*#$*&)tuation']));
        $this->assertEquals('/main/controller/action#fragment', $this->object->build(['controller' => 'controller', 'action' => 'action', '#' => 'fragment']));
    }

    public function testBuildWithExt() {
        $this->assertEquals('/main/index/index.html', $this->object->build(['ext' => 'html']));
        $this->assertEquals('/main/foo/bar.json', $this->object->build(['controller' => 'foo', 'action' => 'bar', 'ext' => 'json']));
        $this->assertEquals('/api/index/index.xml', $this->object->build(['module' => 'api', 'ext' => 'xml']));
    }

    public function testBuildWithArgs() {
        $this->assertEquals('/main/index/index/1/foo/838293', $this->object->build([1, 'foo', 838293]));
        $this->assertEquals('/main/index/index/1/foo/838293', $this->object->build(['args' => [1, 'foo', 838293]]));
        $this->assertEquals('/users/index/action/a/b/3', $this->object->build(['module' => 'users', 'action' => 'action', 'a', 'b', 3]));
        $this->assertEquals('/main/controller/index/a/1337', $this->object->build(['controller' => 'controller', 'args' => ['a'], 1337]));
        $this->assertEquals('/args/with/spaces/a+b+c/special+%3A1%23%24%28%3B%5B%5D+chars', $this->object->build(['module' => 'args', 'controller' => 'with', 'action' => 'spaces', 'args' => ['a b c'], 'special :1#$(;[] chars']));
        $this->assertEquals('/args/with/ext/123/456.html', $this->object->build(['module' => 'args', 'controller' => 'with', 'action' => 'ext', 'ext' => 'html', 123, 456]));
        $this->assertEquals('/args/with/fragment/some+Arg+UMent.json#fragment', $this->object->build(['module' => 'args', 'controller' => 'with', 'action' => 'fragment', 'ext' => 'json', 'some Arg UMent', '#' => 'fragment']));
    }

    public function testBuildWithQuery() {
        $this->assertEquals('/main?foo=bar&number=1', $this->object->build(['foo' => 'bar', 'number' => 1]));
        $this->assertEquals('/main?foo=bar&number=1', $this->object->build(['query' => ['foo' => 'bar', 'number' => 1]]));
        $this->assertEquals('/users/index/action/3?a=b', $this->object->build(['module' => 'users', 'action' => 'action', 'a' => 'b', 3]));
        $this->assertEquals('/main/controller/index/1337?0=a', $this->object->build(['controller' => 'controller', 'query' => ['a'], 1337]));
        $this->assertEquals('/query/with/spaces?a=1&special=%3A1%23%24%28%3B%5B%5D+chars', $this->object->build(['module' => 'query', 'controller' => 'with', 'action' => 'spaces', 'query' => ['a' => 1, 'special' => ':1#$(;[] chars']]));
        $this->assertEquals('/query/with/ext.html?int=456&foo=bar', $this->object->build(['module' => 'query', 'controller' => 'with', 'action' => 'ext', 'ext' => 'html', 'int' => 456, 'foo' => 'bar']));
        $this->assertEquals('/query/with/fragment.json?0=some+Arg+UMent#fragment', $this->object->build(['module' => 'query', 'controller' => 'with', 'action' => 'fragment', 'ext' => 'json', 'query' => 'some Arg UMent', '#' => 'fragment']));
    }

    public function testBuildWithFragment() {
        $this->assertEquals('/main#frag+ment', $this->object->build(['#' => 'frag ment']));
        $this->assertEquals('/main#foo=bar&number=1', $this->object->build(['#' => ['foo' => 'bar', 'number' => 1]]));
        $this->assertEquals('/main/index/action#frag+ment', $this->object->build(['#' => 'frag ment', 'action' => 'action']));
        $this->assertEquals('/main/index/action.html#foo=bar&number=1', $this->object->build(['#' => ['foo' => 'bar', 'number' => 1], 'action' => 'action', 'ext' => 'html']));
        $this->assertEquals('/main/index/action/123/abc#fragment', $this->object->build(['#' => 'fragment', 'action' => 'action', 123, 'abc']));
        $this->assertEquals('/main/index/action/123/abc.html#foo=bar&number=1', $this->object->build(['#' => ['foo' => 'bar', 'number' => 1], 'action' => 'action', 'ext' => 'html', 'args' => [123], 'abc']));
        $this->assertEquals('/main/index/action/123/abc?foo=bar&int=123#fragment', $this->object->build(['#' => 'fragment', 'action' => 'action', 123, 'abc', 'foo' => 'bar', 'int' => 123]));
        $this->assertEquals('/main/index/action/123/abc.html?foo=bar&int=123#foo=bar&number=1', $this->object->build(['#' => ['foo' => 'bar', 'number' => 1], 'action' => 'action', 'ext' => 'html', 'args' => [123], 'abc', 'query' => ['foo' => 'bar'], 'int' => 123]));
    }

    public function testBuildWithLocale() {
        Config::set('titon.locale.current', 'en_US');

        $this->assertEquals('/en-us/forum', $this->object->build(['module' => 'forum']));

        Config::remove('titon.locale.current');
    }

    public function testBuildFormats() {
        $this->object->map('users.login', new Route('/login', ['module' => 'users', 'controller' => 'log', 'action' => 'in']));
        $this->object->map('profile', new Route('/profile/{username}', ['module' => 'profiles']));
        $this->object->map('blog.archives', new Route('/blog/[year]/[month]/[day]', 'Blog\Api@archives'));

        $this->assertEquals('http://google.com', $this->object->build('http://google.com'));

        // by array
        $this->assertEquals('/module', $this->object->build(['module' => 'module']));
        $this->assertEquals('/main/f-o-o-/b-a-r/b/a/z', $this->object->build(['controller' => 'f o o ', 'action' => 'b a r', 'b', 'a', 'z']));

        // by key
        $this->assertEquals('/login', $this->object->build('users.login'));
        $this->assertEquals('/profile/miles', $this->object->build(['route' => 'profile', 'username' => 'miles']));
        $this->assertEquals('/profile/foo', $this->object->build(['username' => 'foo', 'route' => 'profile']));
        $this->assertEquals('/profile/bar/567#fragment', $this->object->build(['username' => 'bar', 'route' => 'profile', 567, '#' => 'fragment']));
        $this->assertEquals('/blog/1988/2/26?key=value', $this->object->build(['route' => 'blog.archives', 'day' => 26, 'year' => 1988, 'month' => 2, 'key' => 'value']));

        // by @ format
        $this->assertEquals('/module/controller', $this->object->build('Module\Controller@index'));
        $this->assertEquals('/module/controller/action', $this->object->build('Module\Controller@action'));
        $this->assertEquals('/some-module/some-controller/and-action', $this->object->build('SomeModule\SomeController@andAction'));

        // by @ format with args
        $this->assertEquals('/module/controller?sort=key&order=asc', $this->object->build(['route' => 'Module\Controller@index', 'sort' => 'key', 'order' => 'asc']));
    }

    /**
     * @expectedException \Titon\Route\Exception\InvalidRouteException
     */
    public function testBuildMissingToken() {
        $this->object->map('profile', new Route('/profile/{username}', ['module' => 'profiles']));

        $this->object->build(['route' => 'profile']);
    }

    /**
     * @expectedException \Titon\Route\Exception\InvalidRouteException
     */
    public function testBuildMissingRoute() {
        $this->object->build(['route' => 'foobar']);
    }

    public function testBuildOptionalToken() {
        $this->object->map('blog.archives', new Route('/blog/[year]/[month]/[day?]', 'Blog\Api@archives'));

        $this->assertEquals('/blog/2012/2', $this->object->build(['route' => 'blog.archives', 'year' => 2012, 'month' => 02]));
        $this->assertEquals('/blog/2012/2/26', $this->object->build(['route' => 'blog.archives', 'year' => 2012, 'month' => 02, 'day' => 26]));
        $this->assertEquals('/blog/2012/2', $this->object->build(['route' => 'blog.archives', 'year' => 2012, 'month' => 02]));
    }

    /**
     * @expectedException \Titon\Route\Exception\InvalidRouteException
     */
    public function testBuildOptionalMissingToken() {
        $this->object->map('blog.archives', new Route('/blog/[year]/[month]/[day?]', 'Blog\Api@archives'));

        $this->object->build(['route' => 'blog.archives', 'year' => 2012]);
    }

    public function testBuildInBaseFolder() {
        $_SERVER['DOCUMENT_ROOT'] = '/root';
        $_SERVER['SCRIPT_FILENAME'] = '/root/base/index.php';

        $router = new Router();

        $this->assertEquals('/base', $router->base());
        $this->assertEquals('/base/module', $this->object->build(['module' => 'module']));
    }

    public function testDefaults() {
        $this->assertEquals([
            'module' => 'main',
            'controller' => 'index',
            'action' => 'index',
            'ext' => '',
            'query' => [],
            'args' => []
        ], Router::defaults());

        $this->assertEquals([
            'module' => 'main',
            'controller' => 'controller',
            'action' => 'some-action',
            'ext' => '',
            'query' => [],
            'args' => []
        ], Router::defaults(['controller' => 'controller', 'action' => 'some_action']));

        $this->assertEquals([
            'module' => 'main',
            'controller' => 'dashed-controller',
            'action' => 'someaction',
            'ext' => 'html',
            'query' => [],
            'args' => []
        ], Router::defaults(['controller' => 'dashed-controller', 'action' => 'someAction', 'ext' => 'html']));

        $this->assertEquals([
            'module' => 'underscore-module',
            'controller' => 'index',
            'action' => 'some-action',
            'ext' => '',
            'query' => [],
            'args' => [],
            '#' => 'fragment'
        ], Router::defaults(['module' => 'underscore_module', 'action' => 'some_action', '#' => 'fragment']));

        $this->assertEquals([
            'module' => 'random-module-chars',
            'controller' => 'index',
            'action' => 'index',
            'ext' => 'json',
            'query' => [],
            'args' => [],
            '#' => ['fragment' => 'array']
        ], Router::defaults(['module' => 'ran%$dom-mo(*$#dule_c%(#hars', 'ext' => 'json', '#' => ['fragment' => 'array']]));

        $this->assertEquals([
            'module' => 'main',
            'controller' => 'index',
            'action' => 'index',
            'ext' => '',
            'query' => [],
            'args' => [],
            'foo' => 'bar',
            'int' => 123,
            123,
            'abc'
        ], Router::defaults([123, 'foo' => 'bar', 'abc', 'int' => 123]));

        $this->assertEquals([
            'module' => 'main',
            'controller' => 'index',
            'action' => 'index',
            'ext' => '',
            'query' => ['foo' => 'bar', 'int' => 123],
            'args' => [123, 'abc']
        ], Router::defaults(['query' => ['foo' => 'bar', 'int' => 123], 'args' => [123, 'abc']]));
    }

    public function testParse() {
        $this->assertEquals([
            'module' => 'module',
            'controller' => 'controller',
            'action' => 'index',
            'ext' => '',
            'query' => [],
            'args' => []
        ], Router::parse('Module\Controller'));

        $this->assertEquals([
            'module' => 'module',
            'controller' => 'controller',
            'action' => 'action',
            'ext' => '',
            'query' => [],
            'args' => []
        ], Router::parse('Module\Controller@action'));

        $this->assertEquals([
            'module' => 'module',
            'controller' => 'controller',
            'action' => 'action',
            'ext' => 'ext',
            'query' => [],
            'args' => []
        ], Router::parse('Module\Controller@action.ext'));

        $this->assertEquals([
            'module' => 'two-words',
            'controller' => 'controller',
            'action' => 'multi-word-action',
            'ext' => '',
            'query' => [],
            'args' => []
        ], Router::parse('TwoWords\Controller@multiWordAction'));

        // Without module
        $this->assertEquals([
            'module' => 'main',
            'controller' => 'moduleless-controller',
            'action' => 'multi-word-action',
            'ext' => '',
            'query' => [],
            'args' => []
        ], Router::parse('ModulelessController@multiWordAction'));
    }

    /**
     * @expectedException \Titon\Route\Exception\InvalidRouteException
     */
    public function testParseInvalidRoute() {
        Router::parse('Broken+Route');
    }

    public function testInitializeAndSegmentsAndBase() {
        $_SERVER['DOCUMENT_ROOT'] = '';
        $_SERVER['HTTP_HOST'] = 'localhost';
        $_SERVER['SCRIPT_FILENAME'] = '/index.php';
        $_SERVER['REQUEST_URI'] = '/';

        $router = new Router();
        $this->assertEquals('/', $router->base());
        $this->assertEquals('http://localhost/', $router->url());
        $this->assertEquals('/', $router->getSegment('path'));
        $this->assertTrue(is_array($router->getSegment('query')));
        $this->assertEquals([
            'path' => '/',
            'scheme' => 'http',
            'query' => [],
            'host' => 'localhost',
            'port' => 80
        ], $router->getSegments());

        // module, controller
        $_SERVER['HTTP_HOST'] = 'domain.com';
        $_SERVER['SCRIPT_FILENAME'] = '/index.php';
        $_SERVER['REQUEST_URI'] = '/module/index';

        $router = new Router();
        $this->assertEquals('/', $router->base());
        $this->assertEquals('http://domain.com/module/index', $router->url());
        $this->assertEquals('/module/index', $router->getSegment('path'));
        $this->assertTrue(is_array($router->getSegment('query')));
        $this->assertEquals([
            'path' => '/module/index',
            'scheme' => 'http',
            'query' => [],
            'host' => 'domain.com',
            'port' => 80
        ], $router->getSegments());

        // module, controller, action, ext, base,
        $_SERVER['HTTP_HOST'] = 'sub.domain.com';
        $_SERVER['SCRIPT_FILENAME'] = '/root/dir/index.php';
        $_SERVER['REQUEST_URI'] = '/module/controller/action.html';

        $router = new Router();
        $this->assertEquals('/root/dir', $router->base());
        $this->assertEquals('http://sub.domain.com/root/dir/module/controller/action.html', $router->url());
        $this->assertEquals('/module/controller/action.html', $router->getSegment('path'));
        $this->assertTrue(is_array($router->getSegment('query')));
        $this->assertEquals([
            'path' => '/module/controller/action.html',
            'scheme' => 'http',
            'query' => [],
            'host' => 'sub.domain.com',
            'port' => 80
        ], $router->getSegments());

        // module, controller, action, ext, base, query, https
        $_SERVER['HTTP_HOST'] = 'subber.sub.domain.com';
        $_SERVER['SCRIPT_FILENAME'] = '/rooter/root/dir/index.php'; // query doesn't show up here
        $_SERVER['REQUEST_URI'] = '/module/controller/action.html?foo=bar&int=123';
        $_SERVER['HTTPS'] = 'on';
        $_GET = ['foo' => 'bar', 'int' => 123];

        $router = new Router();
        $this->assertEquals('/rooter/root/dir', $router->base());
        $this->assertEquals('https://subber.sub.domain.com/rooter/root/dir/module/controller/action.html?foo=bar&int=123', $router->url());
        $this->assertEquals('/module/controller/action.html', $router->getSegment('path'));
        $this->assertTrue(is_array($router->getSegment('query')));
        $this->assertEquals([
            'path' => '/module/controller/action.html',
            'scheme' => 'https',
            'query' => ['foo' => 'bar', 'int' => 123],
            'host' => 'subber.sub.domain.com',
            'port' => 80
        ], $router->getSegments());

        // module, controller, action, ext, base, query, https, args
        $_SERVER['HTTP_HOST'] = 'subbest.subber.sub.domain.com';
        $_SERVER['SCRIPT_FILENAME'] = '/base/rooter/root/dir/index.php'; // query doesn't show up here
        $_SERVER['REQUEST_URI'] = '/module/controller/action.html/123/abc?foo=bar&int=123';
        $_SERVER['HTTPS'] = 'on';
        $_GET = ['foo' => 'bar', 'int' => 123];

        $router = new Router();
        $this->assertEquals('/base/rooter/root/dir', $router->base());
        $this->assertEquals('https://subbest.subber.sub.domain.com/base/rooter/root/dir/module/controller/action.html/123/abc?foo=bar&int=123', $router->url());
        $this->assertEquals('/module/controller/action.html/123/abc', $router->getSegment('path'));
        $this->assertTrue(is_array($router->getSegment('query')));
        $this->assertEquals([
            'path' => '/module/controller/action.html/123/abc',
            'scheme' => 'https',
            'query' => ['foo' => 'bar', 'int' => 123],
            'host' => 'subbest.subber.sub.domain.com',
            'port' => 80
        ], $router->getSegments());
    }

    /**
     * @expectedException \Titon\Route\Exception\MissingSegmentException
     */
    public function testGetSegmentMissingKey() {
        $this->object->getSegment('fakeKey');
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

    public function testHttpMapping() {
        $this->object->map('url1', new Route('/url'));
        $this->object->get('url2', new Route('/url'));
        $this->object->post('url3', new Route('/url'));
        $this->object->put('url4', new Route('/url'));
        $this->object->delete('url5', new Route('/url'));

        $routes = $this->object->all();

        $this->assertEquals([], $routes['url1']->getMethod());
        $this->assertEquals(['get', 'head'], $routes['url2']->getMethod());
        $this->assertEquals(['post'], $routes['url3']->getMethod());
        $this->assertEquals(['put'], $routes['url4']->getMethod());
        $this->assertEquals(['delete'], $routes['url5']->getMethod());
    }

    public function testGetRoute() {
        $route = new Route('/');
        $router = new Router();
        $router->map('key', $route);

        $this->assertEquals($route, $router->getRoute('key'));
        $this->assertEquals(['key' => $route], $router->getRoutes());
    }

    /**
     * @expectedException \Titon\Route\Exception\MissingRouteException
     */
    public function testGetRouteMissingKey() {
        $this->object->getRoute('fakeKey');
    }

    public function testResourceMapping() {
        $this->object->resource('rest', new Route('/rest', 'Api\Rest'));

        $routes = $this->object->all();

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
        $this->assertEquals('/rest/(id)', $routes['rest.read']->getPath());
        $this->assertEquals('/rest/(id)', $routes['rest.update']->getPath());
        $this->assertEquals('/rest/(id)', $routes['rest.delete']->getPath());

        // Route
        $this->assertEquals(['module' => 'api', 'controller' => 'rest', 'action' => 'index', 'ext' => '', 'query' => [], 'args' => []], $routes['rest.list']->getParams());
        $this->assertEquals(['module' => 'api', 'controller' => 'rest', 'action' => 'create', 'ext' => '', 'query' => [], 'args' => []], $routes['rest.create']->getParams());
        $this->assertEquals(['module' => 'api', 'controller' => 'rest', 'action' => 'read', 'ext' => '', 'query' => [], 'args' => []], $routes['rest.read']->getParams());
        $this->assertEquals(['module' => 'api', 'controller' => 'rest', 'action' => 'update', 'ext' => '', 'query' => [], 'args' => []], $routes['rest.update']->getParams());
        $this->assertEquals(['module' => 'api', 'controller' => 'rest', 'action' => 'delete', 'ext' => '', 'query' => [], 'args' => []], $routes['rest.delete']->getParams());

        // Method
        $this->assertEquals(['get'], $routes['rest.list']->getMethod());
        $this->assertEquals(['post'], $routes['rest.create']->getMethod());
        $this->assertEquals(['get'], $routes['rest.read']->getMethod());
        $this->assertEquals(['put', 'post'], $routes['rest.update']->getMethod());
        $this->assertEquals(['delete', 'post'], $routes['rest.delete']->getMethod());

        // Pass
        $this->assertEquals([], $routes['rest.list']->getPassed());
        $this->assertEquals([], $routes['rest.create']->getPassed());
        $this->assertEquals(['id'], $routes['rest.read']->getPassed());
        $this->assertEquals(['id'], $routes['rest.update']->getPassed());
        $this->assertEquals(['id'], $routes['rest.delete']->getPassed());
    }

    public function testGroupPrefixing() {
        $this->object->group(['prefix' => '/pre/'], function(Router $router) {
            $router->map('group1', new Route('/group-1'));
            $router->map('group2', new Route('/group-2'));
        });

        $this->object->map('solo', new Route('/solo'));

        $routes = $this->object->all();

        $this->assertEquals('/', $routes['root']->getPath());
        $this->assertEquals('/pre/group-1', $routes['group1']->getPath());
        $this->assertEquals('/pre/group-2', $routes['group2']->getPath());
        $this->assertEquals('/solo', $routes['solo']->getPath());
    }

    public function testGroupSuffixing() {
        $this->object->group(['suffix' => '/post/'], function(Router $router) {
            $router->map('group1', new Route('/group-1'));
            $router->map('group2', new Route('/group-2'));
        });

        $this->object->map('solo', new Route('/solo'));

        $routes = $this->object->all();

        $this->assertEquals('/', $routes['root']->getPath());
        $this->assertEquals('/group-1/post', $routes['group1']->getPath());
        $this->assertEquals('/group-2/post', $routes['group2']->getPath());
        $this->assertEquals('/solo', $routes['solo']->getPath());
    }

    public function testGroupSecure() {
        $this->object->group(['secure' => true], function(Router $router) {
            $router->map('group1', new Route('/group-1'));
            $router->map('group2', new Route('/group-2'));
        });

        $this->object->map('solo', new Route('/solo'));

        $routes = $this->object->all();

        $this->assertEquals(false, $routes['root']->getSecure());
        $this->assertEquals(true, $routes['group1']->getSecure());
        $this->assertEquals(true, $routes['group2']->getSecure());
        $this->assertEquals(false, $routes['solo']->getSecure());
    }

    public function testGroupPatterns() {
        $this->object->group(['prefix' => '<token>', 'patterns' => ['token' => '([abcd]+)']], function(Router $router) {
            $router->map('group1', new Route('/group-1'));
            $router->map('group2', new Route('/group-2', [], ['patterns' => ['foo' => '(bar|baz)']]));
        });

        $this->object->map('solo', new Route('/solo'));

        $routes = $this->object->all();

        $this->assertEquals('/', $routes['root']->getPath());
        $this->assertEquals('/<token>/group-1', $routes['group1']->getPath());
        $this->assertEquals('/<token>/group-2', $routes['group2']->getPath());
        $this->assertEquals('/solo', $routes['solo']->getPath());

        $this->assertEquals([], $routes['root']->getPatterns());
        $this->assertEquals(['token' => '([abcd]+)'], $routes['group1']->getPatterns());
        $this->assertEquals(['foo' => '(bar|baz)', 'token' => '([abcd]+)'], $routes['group2']->getPatterns());
        $this->assertEquals([], $routes['solo']->getPatterns());
    }

    public function testGroupPass() {
        $this->object->group(['suffix' => '[id]', 'pass' => 'id'], function(Router $router) {
            $router->map('group1', new Route('/group-1', [], ['pass' => ['foo', 'id']]));
            $router->map('group2', new Route('/group-2'));
        });

        $this->object->map('solo', new Route('/solo'));

        $routes = $this->object->all();

        $this->assertEquals('/', $routes['root']->getPath());
        $this->assertEquals('/group-1/[id]', $routes['group1']->getPath());
        $this->assertEquals('/group-2/[id]', $routes['group2']->getPath());
        $this->assertEquals('/solo', $routes['solo']->getPath());

        $this->assertEquals([], $routes['root']->getPassed());
        $this->assertEquals(['id', 'foo'], $routes['group1']->getPassed());
        $this->assertEquals(['id'], $routes['group2']->getPassed());
        $this->assertEquals([], $routes['solo']->getPassed());
    }

    public function testNestedGroups() {
        $this->object->group(['prefix' => '/pre/'], function(Router $router) {
            $router->map('group1', new Route('/group-1'));

            $router->group(['suffix' => '/post'], function(Router $router) {
                $router->map('group2', new Route('/group-2'));
            });
        });

        $this->object->map('solo', new Route('/solo'));

        $routes = $this->object->all();

        $this->assertEquals('/', $routes['root']->getPath());
        $this->assertEquals('/pre/group-1', $routes['group1']->getPath());
        $this->assertEquals('/pre/group-2/post', $routes['group2']->getPath());
        $this->assertEquals('/solo', $routes['solo']->getPath());
    }

    public function testFilter() {
        $stub = new FilterStub();

        $this->object->filter('test', function() {});
        $this->object->filter('test2', $stub);

        $this->assertEquals([$stub, 'filter'], $this->object->getFilter('test2'));
        $this->assertEquals(['test', 'test2'], array_keys($this->object->getFilters()));

        // Filtering is passed to routes
        $this->object->map('f1', new Route('/f1', [], ['filters' => ['test']]));
        $this->object->group(['filters' => 'test'], function(Router $router) {
            $router->map('f2', new Route('/f2', []));
        });
        $this->object->map('f3', new Route('/f3', []));

        $routes = $this->object->all();

        $this->assertEquals(['test'], $routes['f1']->getFilters());
        $this->assertEquals(['test'], $routes['f2']->getFilters());
        $this->assertEquals([], $routes['f3']->getFilters());
    }

    /**
     * @expectedException \Titon\Route\Exception\InvalidFilterException
     */
    public function testFilterNonCallable() {
        $this->object->filter('foo', 'bar');
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

        $router->filter('test', function() use (&$count) {
            $count++;
        });

        $router->map('f1', new Route('/f1', [], ['filters' => ['test']]));
        $router->group(['filters' => 'test'], function(Router $router) {
            $router->map('f2', new Route('/f2', []));
        });
        $router->map('f3', new Route('/f3', []));

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
    public function testFilterThrowsException() {
        $this->object->filter('test', function() use (&$count) {
            throw new \Exception('Filter error!');
        });

        $this->object->map('root', new Route('/', [], ['filters' => ['test']]));

        $this->object->match('/');
    }

}

class FilterStub implements Filter {

    public function filter(Router $router, Route $route) {
        return;
    }

}