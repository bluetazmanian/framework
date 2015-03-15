<?hh // strict
namespace Titon\Test\Stub\Kernel;

use Titon\Kernel\Middleware\Next;

class CallNextKernelStub extends KernelStub {
    public function handle(InputStub $input, OutputStub $output, Next<InputStub, OutputStub> $next): OutputStub {
        $next->handle($input, $output);

        if ($output instanceof OutputStub) {
            $output->ran = true;
        }

        // UNSAFE
        return $output;
    }
}
