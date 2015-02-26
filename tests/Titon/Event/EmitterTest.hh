<?hh
namespace Titon\Event;

use Titon\Test\Stub\Event\ListenerStub;
use Titon\Test\TestCase;

/**
 * @property \Titon\Event\Emitter $object
 */
class EmitterTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        $this->object = new Emitter();
    }

    public function testFlush() {
        $ob1 = function(Event $event) { };
        $ob2 = function(Event $event) { };

        $this->object->subscribe('event.test1', $ob1);
        $this->object->subscribe('event.test2', $ob2);

        $this->assertEquals(Vector {'event.test1', 'event.test2'}, $this->object->getEventKeys());

        $this->object->flush('event.test2');
        $this->assertEquals(Vector {'event.test1'}, $this->object->getEventKeys());

        $this->object->flush();
        $this->assertEquals(Vector {}, $this->object->getEventKeys());
    }

    public function testGetObservers() {
        $ob1 = function(Event $event) { };
        $ob2 = [new ListenerStub(), 'noop1'];

        $this->object->subscribe('event.test', $ob1, 20);
        $this->object->subscribe('event.test', $ob2, 15);

        $this->assertEquals(Vector {}, $this->object->getObservers('event.foobar'));
        $this->assertEquals(Vector {
            new Observer($ob1, 20, false),
            new Observer($ob2, 15, false),
        }, $this->object->getObservers('event.test'));
    }

    public function testGetCallStack() {
        $ob1 = function(Event $event) { };
        $ob2 = [new ListenerStub(), 'noop2'];
        $ob3 = function(Event $event) { };
        $ob4 = [new ListenerStub(), 'noop3'];

        $this->object->subscribe('event.test', $ob1, 20);
        $this->object->subscribe('event.test', $ob2, 25);
        $this->object->subscribe('event.test', $ob3, 5);
        $this->object->subscribe('event.test', $ob4, 75);

        $this->assertEquals(Vector {
            '{closure}',
            '{closure}',
            'Titon\Test\Stub\Event\ListenerStub::noop2',
            'Titon\Test\Stub\Event\ListenerStub::noop3'
        }, $this->object->getCallStack('event.test'));
    }

    public function testGetEventKeys() {
        $this->assertEquals(Vector {}, $this->object->getEventKeys());

        $this->object->subscribe('event.test1', function(Event $event) { });
        $this->object->subscribe('event.test2', function(Event $event) { });

        $this->assertEquals(Vector {'event.test1', 'event.test2'}, $this->object->getEventKeys());
    }

    public function testGetSortedObservers() {
        $ob1 = function(Event $event) { };
        $ob2 = [new ListenerStub(), 'noop1'];
        $ob3 = function(Event $event) { };

        $this->object->subscribe('event.test', $ob1, 20);
        $this->object->subscribe('event.test', $ob2, 15);
        $this->object->subscribe('event.test', $ob3, 20);

        $this->assertEquals(Vector {}, $this->object->getObservers('event.foobar'));
        $this->assertEquals(Vector {
            new Observer($ob1, 20, false),
            new Observer($ob2, 15, false),
            new Observer($ob3, 20, false),
        }, $this->object->getObservers('event.test'));

        // Sorted
        $this->assertEquals(Vector {
            new Observer($ob2, 15, false),
            new Observer($ob1, 20, false),
            new Observer($ob3, 20, false),
        }, $this->object->getSortedObservers('event.test'));
    }

    public function testHasObservers() {
        $this->assertFalse($this->object->hasObservers('event.test'));

        $this->object->subscribe('event.test', function(Event $event) { });

        $this->assertTrue($this->object->hasObservers('event.test'));
    }

    public function testAsyncGetObservers() {
        $stub = new ListenerStub();

        $this->object->subscribe('event.test', [$stub, 'noop1']);
        $this->object->subscribe('event.test', [$stub, 'asyncNoop1']);

        $observers = $this->object->getObservers('event.test');

        $this->assertEquals(2, count($observers));
        $this->assertFalse($observers[0]->isAsync());
        $this->assertTrue($observers[1]->isAsync());
    }

    public function testExit() {
        $n = 0;
        $ob1 = function(Event $event, &$i) { $i++; };
        $ob2 = function(Event $event, &$i) { $i++; return true; };
        $ob3 = function(Event $event, &$i) { $i++; return false; };
        $ob4 = function(Event $event, &$i) { $i++; };

        $this->object->subscribe('event.test', $ob1);
        $this->object->subscribe('event.test', $ob2);
        $this->object->subscribe('event.test', $ob3);
        $this->object->subscribe('event.test', $ob4);

        $this->object->emit('event.test', [&$n]);

        $this->assertEquals(3, $n);
    }

    public function testData() {
        $ob1 = function(Event $event) { $event->setData('key', $event->getData('key') + 1); };
        $ob2 = function(Event $event) { $event->setData('key', $event->getData('key') + 1); };
        $ob3 = function(Event $event) { $event->setData('key', $event->getData('key') + 1); };
        $ob4 = function(Event $event) { $event->setData('key', $event->getData('key') + 1); };

        $this->object->subscribe('event.test', $ob1);
        $this->object->subscribe('event.test', $ob2);
        $this->object->subscribe('event.test', $ob3);
        $this->object->subscribe('event.test', $ob4);

        $event = $this->object->emit('event.test', []);

        $this->assertEquals(4, $event->getData('key'));
        $this->assertEquals(Map {'key' => 4}, $event->getData());
    }

    public function testSubscribeAndUnsubscribe() {
        $ob1 = function(Event $event) { };
        $ob2 = function(Event $event) { };

        $this->object->subscribe('event.test', $ob1);
        $this->object->subscribe('event.test', $ob2);

        $this->assertEquals(Vector {
            new Observer($ob1, 100, false),
            new Observer($ob2, 101, false)
        }, $this->object->getObservers('event.test'));

        // Remove using the instance
        $this->object->unsubscribe('event.test', $ob1);

        $this->assertEquals(Vector {
            new Observer($ob2, 101, false)
        }, $this->object->getObservers('event.test'));
    }

    public function testListenAndUnlisten() {
        $listener = new ListenerStub();

        $this->object->listen($listener);

        $this->assertEquals(Vector {
            'Titon\Test\Stub\Event\ListenerStub::noop2',
            'Titon\Test\Stub\Event\ListenerStub::noop1'
        }, $this->object->getCallStack('event.test1'));

        // Remove using the instance
        $this->assertEquals(Vector {'event.test1', 'event.test2', 'event.test3'}, $this->object->getEventKeys());

        $this->object->unlisten($listener);

        $this->assertEquals(Vector {}, $this->object->getCallStack('event.test1'));
    }

    public function testEmit() {
        $ob1 = function(Event $event) { };
        $ob2 = function(Event $event) { $event->stop(); };
        $ob3 = new ListenerStub();

        $this->object->subscribe('event.test', $ob1);
        $this->object->subscribe('event.test', $ob2);
        $this->object->subscribe('event.test', [$ob3, 'noop3']);

        $event = $this->object->emit('event.test', []);

        $this->assertTrue($event->isStopped());
        $this->assertEquals(1, $event->getIndex());
    }

    public function testEmitNoObservers() {
        $event = $this->object->emit('fake.event', []);

        $this->assertInstanceOf('Titon\Event\Event', $event);
        $this->assertEquals(Vector {}, $event->getCallStack());
    }

    public function testEmitParams() {
        $ob1 = new ListenerStub();
        $ob2 = function(Event $event, &$int, $object) { $int++; };
        $ob3 = function(Event $event, &$int, $object) { $int += 5; };

        $this->object->subscribe('event.test', [$ob1, 'noop3']);
        $this->object->subscribe('event.test', $ob2);
        $this->object->subscribe('event.test', $ob3);

        $int = 0;
        $object = new \stdClass();
        $object->foo = 'baz';

        $this->object->emit('event.test', [&$int, $object]);

        $this->assertEquals('bar', $object->foo);
        $this->assertEquals(6, $int);
    }

    public function testEmitReturnState() {
        $count = 0;
        $ob1 = function(Event $event) use (&$count) { $count++; }; // Void, will continue
        $ob2 = function(Event $event) use (&$count) { $count++; return true; }; // True, will continue
        $ob3 = function(Event $event) use (&$count) { $count++; return ['foo' => 'bar']; }; // Has a value, will stop
        $ob4 = function(Event $event) use (&$count) { $count++; return 'foobar'; }; // Will not be reached

        $this->object->subscribe('event.test', $ob1);
        $this->object->subscribe('event.test', $ob2);
        $this->object->subscribe('event.test', $ob3);
        $this->object->subscribe('event.test', $ob4);

        $event = $this->object->emit('event.test', []);

        $this->assertEquals(3, $count);
        $this->assertEquals(['foo' => 'bar'], $event->getState());
    }

    public function testEmitMany() {
        $ob1 = function(Event $event) { };
        $ob2 = function(Event $event) { $event->stop(); };
        $ob3 = new ListenerStub();

        $this->object->subscribe('event.test', $ob1);
        $this->object->subscribe('event.test', $ob2);
        $this->object->listen($ob3);

        $events = $this->object->emitMany('event.test event.test1', []);

        $this->assertEquals(2, count($events));
    }

    public function testEmitManyWildcard() {
        $ob1 = function(Event $event) { };
        $ob2 = function(Event $event) { $event->stop(); };
        $ob3 = new ListenerStub();

        $this->object->subscribe('event.cb1', $ob1);
        $this->object->subscribe('event.cb2', $ob2);
        $this->object->listen($ob3);

        $events = $this->object->emitMany('event.*', []);
        $this->assertEquals(5, count($events));
    }

    public function testEmitAsyncs() {
        $stub = new ListenerStub();
        $list = [];

        $this->object->subscribe('event.test', [$stub, 'asyncNoop1']);
        $this->object->subscribe('event.test', [$stub, 'asyncNoop2']);
        $this->object->subscribe('event.test', [$stub, 'asyncNoop3']);
        $this->object->subscribe('event.test', [$stub, 'asyncNoop1']);
        $this->object->subscribe('event.test', [$stub, 'asyncNoop2']);
        $this->object->subscribe('event.test', [$stub, 'asyncNoop3']);

        $this->assertEquals(6, count($this->object->getObservers('event.test')));

        $this->object->emit('event.test', [&$list]);

        $this->assertEquals(6, count($list));
        $this->assertNotEquals([1, 2, 3, 1, 2, 3], $list);
    }

}
