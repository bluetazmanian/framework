<?hh // strict
namespace Titon\Test\Stub\Kernel;

use Titon\Kernel\Middleware;
use Titon\Kernel\Middleware\Next;

class MiddlewareStub implements Middleware {
    public function __construct(protected string $key): void {}

    public function handle<Ti, To>(Ti $input, To $output, Next $next): To {
        if ($input instanceof InputStub) {
            $input->stack[] = $this->key;
        }

        $output = $next->handle($input, $output);

        if ($input instanceof InputStub) {
            $input->stack[] = $this->key;
        }

        return $output;
    }
}
