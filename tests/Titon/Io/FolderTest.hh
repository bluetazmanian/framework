<?hh
namespace Titon\Io;

use Titon\Test\TestCase;
use VirtualFileSystem\FileSystem;

/**
 * @property \Titon\Io\Folder $object
 */
class FolderTest extends TestCase {

    /** @type \Titon\Io\Folder */
    protected $temp;

    /** @type int */
    protected $userid;

    /** @type int */
    protected $groupid;

    protected function setUp(): void {
        parent::setUp();

        $this->vfs = new FileSystem();
        $this->object = new Folder($this->vfs()->path('base/'), true);
        $this->temp = new Folder($this->vfs()->path('temp/'), false);

        // Match VFS
        $this->userid = function_exists('posix_getuid') ? posix_getuid() : 0;
        $this->groupid = function_exists('posix_getgid') ? posix_getgid() : 0;
    }

    /**
     * @expectedException \Titon\Io\Exception\InvalidPathException
     */
    public function testConstructThrowsNotFolderException(): void {
        new Folder(TEST_DIR . '/bootstrap.php');
    }

    public function testAccessTime(): void {
        $this->assertTrue(is_int($this->object->accessTime()));
        $this->assertEquals(0, $this->temp->accessTime());
    }

    public function testBasename(): void {
        $this->assertEquals('base', $this->object->basename());
        $this->assertEquals('temp', $this->temp->basename());
    }

    public function testCd(): void {
        $this->assertEquals($this->vfs()->path('base/'), $this->object->path());
        $this->object->cd($this->vfs()->path('base-cd/'));
        $this->assertEquals($this->vfs()->path('base-cd/'), $this->object->path());
    }

    public function testChangeTime(): void {
        $this->assertTrue(is_int($this->object->changeTime()));
        $this->assertEquals(0, $this->temp->changeTime());
    }

    public function testChgrp(): void {
        $file = new Folder($this->vfs()->path('base/chgrp'), true);

        $this->assertEquals($this->groupid, $file->group());
        $this->assertTrue($file->chgrp(666));
        $this->assertEquals(666, $file->group());
    }

    public function testChgrpLink(): void {
        $this->vfs()->createDirectory('base/link');
        $this->vfs()->createLink('base/chgrp-link', 'base/link');

        $file = new Folder($this->vfs()->path('base/chgrp-link'));

        $this->assertTrue(is_link($file->path()));
        $this->assertEquals($this->groupid, $file->group());
        $this->assertTrue($file->chgrp(666));
        $this->assertEquals(666, $file->group());
    }

    public function testChgrpMissingFile(): void {
        $this->assertFalse($this->temp->chgrp(666));
    }

    public function testChgrpRecursive(): void {
        $folder = new Folder($this->vfs()->path('base/chgrp'), true);
        $file1 = new File($this->vfs()->path('base/chgrp/1'), true);
        $file2 = new Folder($this->vfs()->path('base/chgrp/2'), true);

        $this->assertEquals($this->groupid, $folder->group());
        $this->assertEquals($this->groupid, $file1->group());
        $this->assertEquals($this->groupid, $file2->group());

        $this->assertTrue($folder->chgrp(666, true));

        $this->assertEquals(666, $folder->group());
        $this->assertEquals(666, $file1->group());
        $this->assertEquals(666, $file2->group());
    }

    public function testChmod(): void {
        $file = new Folder($this->vfs()->path('base/chmod'), true);

        $this->assertEquals(755, $file->permissions());
        $this->assertTrue($file->chmod(0666));
        $this->assertEquals(666, $file->permissions());
    }

    public function testChmodMissingFile(): void {
        $this->assertFalse($this->temp->chmod(0666));
    }

    public function testChmodRecursive(): void {
        $folder = new Folder($this->vfs()->path('base/chmod'), true);
        $file1 = new File($this->vfs()->path('base/chmod/1'), true);
        $file2 = new Folder($this->vfs()->path('base/chmod/2'), true);

        $this->assertEquals(755, $folder->permissions());
        $this->assertEquals(755, $file1->permissions());
        $this->assertEquals(755, $file2->permissions());

        $this->assertTrue($folder->chmod(0666, true));

        $this->assertEquals(666, $folder->permissions());
        $this->assertEquals(666, $file1->permissions());
        $this->assertEquals(666, $file2->permissions());
    }

    public function testChown(): void {
        $file = new Folder($this->vfs()->path('base/chown'), true);

        $this->assertEquals($this->userid, $file->owner());
        $this->assertTrue($file->chown(666));
        $this->assertEquals(666, $file->owner());
    }

    public function testChownLink(): void {
        $this->vfs()->createDirectory('base/link');
        $this->vfs()->createLink('base/chown-link', 'base/link');

        $file = new Folder($this->vfs()->path('base/chown-link'));

        $this->assertTrue(is_link($file->path()));
        $this->assertEquals($this->userid, $file->owner());
        $this->assertTrue($file->chown(666));
        $this->assertEquals(666, $file->owner());
    }

    public function testChownMissingFile(): void {
        $this->assertFalse($this->temp->chown(666));
    }

    public function testChownRecursive(): void {
        $folder = new Folder($this->vfs()->path('base/chown'), true);
        $file1 = new File($this->vfs()->path('base/chown/1'), true);
        $file2 = new Folder($this->vfs()->path('base/chown/2'), true);

        $this->assertEquals($this->userid, $folder->owner());
        $this->assertEquals($this->userid, $file1->owner());
        $this->assertEquals($this->userid, $file2->owner());

        $this->assertTrue($folder->chown(666, true));

        $this->assertEquals(666, $folder->owner());
        $this->assertEquals(666, $file1->owner());
        $this->assertEquals(666, $file2->owner());
    }

    public function testCreate(): void {
        $this->assertTrue($this->object->exists());
        $this->assertFalse($this->object->create());

        $this->assertFalse($this->temp->exists());
        $this->assertTrue($this->temp->create());
        $this->assertTrue($this->temp->exists());
    }

    public function testCopy(): void {
        $this->vfs()->createStructure([
            'base/copy' => [
                '1' => 'foo',
                '2' => 'foo'
            ]
        ]);

        $this->assertFileNotExists($this->vfs()->path('base/copy-to'));
        $this->assertFileNotExists($this->vfs()->path('base/copy-to/1'));

        $baseFolder = new Folder($this->vfs()->path('base/copy'));
        $baseFolder->copy($this->vfs()->path('base/copy-to'));

        $this->assertFileExists($this->vfs()->path('base/copy-to'));
        $this->assertFileExists($this->vfs()->path('base/copy-to/1'));
        $this->assertEquals('foo', file_get_contents($this->vfs()->path('base/copy-to/2')));
    }

    public function testCopyMissingFile(): void {
        $this->assertEquals(null, $this->temp->copy($this->vfs()->path('base/copy-to')));
    }

    public function testCopyOverwrite(): void {
        $this->vfs()->createStructure([
            'base/copy' => [
                '1' => 'foo',
                '2' => 'foo'
            ],
            'base/copy-to' => [
                '2' => 'bar',
                '3' => [
                    '3-1' => 'bar'
                ]
            ]
        ]);

        $this->assertFileNotExists($this->vfs()->path('base/copy-to/1'));
        $this->assertFileExists($this->vfs()->path('base/copy-to/3/3-1'));

        $baseFolder = new Folder($this->vfs()->path('base/copy'));
        $baseFolder->copy($this->vfs()->path('base/copy-to'), Folder::OVERWRITE);

        $this->assertFileExists($this->vfs()->path('base/copy-to/1'));
        $this->assertFileNotExists($this->vfs()->path('base/copy-to/3/3-1'));
        $this->assertEquals('foo', file_get_contents($this->vfs()->path('base/copy-to/2')));
    }

    public function testCopySkip(): void {
        $this->vfs()->createStructure([
            'base/copy' => [
                '1' => 'foo',
                '2' => 'foo',
                '4' => [
                    '4-3' => 'foo'
                ]
            ],
            'base/copy-to' => [
                '2' => 'bar',
                '3' => [
                    '3-1' => 'bar'
                ]
            ]
        ]);

        $this->assertFileNotExists($this->vfs()->path('base/copy-to/1'));
        $this->assertFileNotExists($this->vfs()->path('base/copy-to/4/4-3'));
        $this->assertEquals('bar', file_get_contents($this->vfs()->path('base/copy-to/2')));

        $baseFolder = new Folder($this->vfs()->path('base/copy'));
        $baseFolder->copy($this->vfs()->path('base/copy-to'), Folder::SKIP);

        $this->assertFileExists($this->vfs()->path('base/copy-to/1'));
        $this->assertFileExists($this->vfs()->path('base/copy-to/4/4-3'));
        $this->assertEquals('bar', file_get_contents($this->vfs()->path('base/copy-to/2')));
    }

    public function testCopyMerge(): void {
        $this->vfs()->createStructure([
            'base/copy' => [
                '1' => 'foo',
                '2' => 'foo',
                '3' => [
                    '3-1' => 'foo',
                    '3-2' => 'foo'
                ],
                '4' => [
                    '4-3' => 'foo'
                ]
            ],
            'base/copy-to' => [
                '2' => 'bar',
                '3' => [
                    '3-1' => 'bar'
                ]
            ]
        ]);

        $this->assertFileNotExists($this->vfs()->path('base/copy-to/1'));
        $this->assertFileExists($this->vfs()->path('base/copy-to/3/3-1'));
        $this->assertFileNotExists($this->vfs()->path('base/copy-to/3/3-2'));
        $this->assertFileNotExists($this->vfs()->path('base/copy-to/4/4-3'));
        $this->assertEquals('bar', file_get_contents($this->vfs()->path('base/copy-to/2')));

        $baseFolder = new Folder($this->vfs()->path('base/copy'));
        $baseFolder->copy($this->vfs()->path('base/copy-to'), Folder::MERGE);

        $this->assertFileExists($this->vfs()->path('base/copy-to/1'));
        $this->assertFileExists($this->vfs()->path('base/copy-to/3/3-1'));
        $this->assertFileExists($this->vfs()->path('base/copy-to/3/3-2'));
        $this->assertFileExists($this->vfs()->path('base/copy-to/4/4-3'));
        $this->assertEquals('foo', file_get_contents($this->vfs()->path('base/copy-to/2')));
        $this->assertEquals('foo', file_get_contents($this->vfs()->path('base/copy-to/3/3-1')));
    }

    public function testDelete(): void {
        $this->assertTrue($this->object->delete());
        $this->assertFalse($this->temp->delete());
    }

    public function testDir(): void {
        $folder = new Folder($this->vfs()->path('base/child/dir'), true);

        $this->assertEquals($this->vfs()->path('base/child/'), $folder->dir());
        $this->assertEquals($this->vfs()->path('base/'), $folder->parent()?->dir());
    }

    public function testExists(): void {
        $this->assertTrue($this->object->exists());
        $this->assertFalse($this->temp->exists());

        $this->temp->create();
        $this->assertTrue($this->temp->exists());
    }

    public function testFiles(): void {
        $this->vfs()->createStructure([
            'base/read' => [
                '4' => 'd',
                '5' => 'e',
                '6' => 'f',
                'child' => [
                    '1' => 'a',
                    '2' => 'b',
                    '3' => 'c'
                ],
                '1' => 'a',
                '2' => 'b',
                '3' => 'c',
            ]
        ]);

        $folder = new Folder($this->vfs()->path('base/read'));
        $contents = $folder->files();

        $scheme = $this->vfs()->scheme();
        $paths = $contents->map(($value) ==> {
            return str_replace($scheme . '://', '', $value->path());
        });

        $this->assertEquals(6, count($contents));
        $this->assertEquals(Vector {
            'base/read/4',
            'base/read/5',
            'base/read/6',
            'base/read/1',
            'base/read/2',
            'base/read/3',
        }, $paths);

        $this->assertEquals(Vector {}, $this->temp->read());
    }

    public function testFind(): void {
        $this->markTestSkipped('Glob iteration does not work with PHP streams');

        $this->vfs()->createStructure([
            'base/find' => [
                'foo.php' => 'a',
                'bar.php' => 'b',
                'baz.php' => 'c',
                'qux.hh' => 'd',
                'sub' => [
                    'foo.txt' => 'e',
                    'bar.hh' => 'f',
                ]
            ]
        ]);

        $object = new Folder($this->vfs()->path('base/find'));

        $files = $object->find('*.php');
        $this->assertTrue(count($files) >= 3);

        $files = $object->find('sub');
        $this->assertTrue(count($files) == 1);

        $files = $object->find('sub/*.php');
        $this->assertTrue(count($files) == 0);

        $files = $object->find('sub/*.txt');
        $this->assertTrue(count($files) == 1);
    }

    public function testFindMissingFile(): void {
        $this->assertEquals(Vector {}, $this->temp->find('*'));
    }

    public function testFolders(): void {
        $this->vfs()->createStructure([
            'base/read' => [
                '4' => 'd',
                '5' => 'e',
                '6' => 'f',
                'child1' => [
                    '1' => 'a',
                    '2' => 'b',
                    '3' => 'c'
                ],
                '1' => 'a',
                'child2' => [
                    '1' => 'a',
                    '2' => 'b',
                    '3' => 'c'
                ],
            ]
        ]);

        $folder = new Folder($this->vfs()->path('base/read'));
        $contents = $folder->folders();

        $scheme = $this->vfs()->scheme();
        $paths = $contents->map(($value) ==> {
            return str_replace($scheme . '://', '', $value->path());
        });

        $this->assertEquals(2, count($contents));
        $this->assertEquals(Vector {
            'base/read/child1/',
            'base/read/child2/',
        }, $paths);

        $this->assertEquals(Vector {}, $this->temp->read());
    }

    public function testGroup(): void {
        $this->assertEquals($this->groupid, $this->object->group());
        $this->assertEquals(null, $this->temp->group());
    }

    public function testIsAbsolute(): void {
        $linux = new Folder('/dir');
        $windows = new Folder('C:\dir');
        $relative = new Folder('../dir');

        $this->assertTrue($this->object->isAbsolute());
        $this->assertTrue($linux->isAbsolute());
        $this->assertTrue($windows->isAbsolute());
        $this->assertFalse($relative->isAbsolute());
    }

    public function testIsRelative(): void {
        $linux = new Folder('../dir');
        $windows = new Folder('..\dir');
        $absolute = new Folder('/dir');

        $this->assertFalse($this->object->isRelative());
        $this->assertTrue($linux->isRelative());
        $this->assertTrue($windows->isRelative());
        $this->assertFalse($absolute->isRelative());
    }

    public function testModifyTime(): void {
        $this->assertTrue(is_int($this->object->modifyTime()));
        $this->assertEquals(0, $this->temp->modifyTime());
    }

    public function testMove(): void {
        $this->vfs()->createStructure([
            'base/move' => [
                '1' => 'foo',
                '2' => 'bar'
            ]
        ]);

        $folder = new Folder($this->vfs()->path('base/move'));

        $this->assertFileExists($this->vfs()->path('base/move'));
        $this->assertFileNotExists($this->vfs()->path('base/move-to'));

        $folder->move($this->vfs()->path('base/move-to'));

        $this->assertEquals($this->vfs()->path('base/move-to/'), $folder->path());
        $this->assertFileNotExists($this->vfs()->path('base/move'));
        $this->assertFileExists($this->vfs()->path('base/move-to'));
    }

    public function testMoveMissingFile(): void {
        $this->assertFalse($this->temp->move($this->vfs()->path('base/move-to')));
    }

    public function testMoveExitsEarlySameLocation(): void {
        $folder = new Folder($this->vfs()->path('base/move'), true);

        $this->assertTrue($folder->move($this->vfs()->path('base/move')));
    }

    /**
     * @expectedException \Titon\Io\Exception\ExistingFileException
     */
    public function testMoveTargetExists(): void {
        $this->vfs()->createStructure([
            'base/move' => [
                '1' => 'foo',
                '2' => 'foo'
            ],
            'base/move-to' => [
                '3' => 'bar'
            ]
        ]);

        $folder = new Folder($this->vfs()->path('base/move'));
        $folder->move($this->vfs()->path('base/move-to'), false);
    }

    public function testMoveOverwriteTargetFile(): void {
        $this->vfs()->createStructure([
            'base/move' => [
                '1' => 'foo'
            ],
            'base/move-to' => 'bar'
        ]);

        $file = new Folder($this->vfs()->path('base/move'));

        $this->assertFileExists($this->vfs()->path('base/move'));
        $this->assertFileExists($this->vfs()->path('base/move-to'));
        $this->assertTrue(is_file($this->vfs()->path('base/move-to')));

        $file->move($this->vfs()->path('base/move-to'), true);

        $this->assertFileNotExists($this->vfs()->path('base/move'));
        $this->assertFileExists($this->vfs()->path('base/move-to'));
        $this->assertFalse(is_file($this->vfs()->path('base/move-to')));
    }

    public function testMoveOverwriteTargetFolder(): void {
        $this->vfs()->createStructure([
            'base/move' => [
                '1' => 'foo'
            ],
            'base/move-to' => [
                '2' => 'bar'
            ]
        ]);

        $file = new Folder($this->vfs()->path('base/move'));

        $this->assertFileExists($this->vfs()->path('base/move'));
        $this->assertFileExists($this->vfs()->path('base/move-to'));
        $this->assertFileExists($this->vfs()->path('base/move-to/2'));
        $this->assertTrue(is_dir($this->vfs()->path('base/move-to')));

        $file->move($this->vfs()->path('base/move-to'), true);

        $this->assertFileNotExists($this->vfs()->path('base/move'));
        $this->assertFileExists($this->vfs()->path('base/move-to'));
        $this->assertFileNotExists($this->vfs()->path('base/move/2'));
        $this->assertTrue(is_dir($this->vfs()->path('base/move-to')));
    }

    public function testName(): void {
        $this->assertEquals('base', $this->object->name());
        $this->assertEquals('temp', $this->temp->name());
    }

    public function testOwner(): void {
        $this->assertEquals($this->userid, $this->object->owner());
        $this->assertEquals(null, $this->temp->owner());
    }

    public function testPath(): void {
        $this->assertEquals($this->vfs()->path('/base/'), $this->object->path());
        $this->assertEquals($this->vfs()->path('/temp/'), $this->temp->path());
    }

    public function testParent(): void {
        $this->assertInstanceOf('Titon\Io\Folder', $this->object->parent());
        $this->assertEquals($this->vfs()->scheme() . ':/', $this->object->parent()->path());
    }

    public function testPermissions(): void {
        $this->assertEquals('0755', $this->object->permissions());
        $this->assertEquals(null, $this->temp->permissions());

        $this->object->chmod(0555);
        $this->temp->chmod(0555);

        $this->assertEquals('0555', $this->object->permissions());
        $this->assertEquals(null, $this->temp->permissions());
    }

    public function testPermissionReading(): void {
        $this->markTestSkipped('This currently fails on HHVM 3.4.0, but works on nightlies');

        $this->object->chmod(0777);
        $this->temp->chmod(0777);

        $this->assertFalse($this->object->executable()); // Folders can't be executable
        $this->assertTrue($this->object->readable());
        $this->assertTrue($this->object->writable());

        $this->assertFalse($this->temp->executable());
        $this->assertFalse($this->temp->readable());
        $this->assertFalse($this->temp->writable());

        $this->object->chmod(0);
        $this->temp->chmod(0);

        $this->assertFalse($this->object->executable());
        $this->assertFalse($this->object->readable());
        $this->assertFalse($this->object->writable());

        $this->assertFalse($this->temp->executable());
        $this->assertFalse($this->temp->readable());
        $this->assertFalse($this->temp->writable());
    }

    public function testPwd(): void {
        $this->assertEquals($this->vfs()->path('base/'), $this->object->path());
        $this->assertEquals($this->vfs()->path('temp/'), $this->temp->path());
    }

    public function testRead(): void {
        $this->vfs()->createStructure([
            'base/read' => [
                '4' => 'd',
                '5' => 'e',
                '6' => 'f',
                'child' => [
                    '1' => 'a',
                    '2' => 'b',
                    '3' => 'c'
                ],
                '1' => 'a',
                '2' => 'b',
                '3' => 'c',
            ]
        ]);

        $folder = new Folder($this->vfs()->path('base/read'));
        $contents = $folder->read();

        $scheme = $this->vfs()->scheme();
        $paths = $contents->map(($value) ==> {
            return str_replace($scheme . '://', '', $value->path());
        });

        $this->assertEquals(7, count($contents));
        $this->assertEquals(Vector {
            'base/read/4',
            'base/read/5',
            'base/read/6',
            'base/read/child/',
            'base/read/1',
            'base/read/2',
            'base/read/3',
        }, $paths);

        $this->assertEquals(Vector {}, $this->temp->read());
    }

    public function testReadRecursive(): void {
        $this->vfs()->createStructure([
            'base/read' => [
                '4' => 'd',
                '5' => 'e',
                '6' => 'f',
                'child' => [
                    '1' => 'a',
                    '2' => 'b',
                    '3' => 'c'
                ],
                '1' => 'a',
                '2' => 'b',
                '3' => 'c',
            ]
        ]);

        $folder = new Folder($this->vfs()->path('base/read'));
        $contents = $folder->read(false, true);

        $scheme = $this->vfs()->scheme();
        $paths = $contents->map(($value) ==> {
            return str_replace($scheme . '://', '', $value->path());
        });

        $this->assertEquals(10, count($contents));
        $this->assertEquals(Vector {
            'base/read/4',
            'base/read/5',
            'base/read/6',
            'base/read/child/1',
            'base/read/child/2',
            'base/read/child/3',
            'base/read/child/',
            'base/read/1',
            'base/read/2',
            'base/read/3',
        }, $paths);

        $this->assertEquals(Vector {}, $this->temp->read());
    }

    public function testReadSorting(): void {
        $this->vfs()->createStructure([
            'base/read' => [
                '4' => 'd',
                '5' => 'e',
                '6' => 'f',
                'child' => [
                    '1' => 'a',
                    '2' => 'b',
                    '3' => 'c'
                ],
                '1' => 'a',
                '2' => 'b',
                '3' => 'c',
            ]
        ]);

        $folder = new Folder($this->vfs()->path('base/read'));
        $contents = $folder->read(true);

        $scheme = $this->vfs()->scheme();
        $paths = $contents->map(($value) ==> {
            return str_replace($scheme . '://', '', $value->path());
        });

        $this->assertEquals(7, count($contents));
        $this->assertEquals(Vector {
            'base/read/1',
            'base/read/2',
            'base/read/3',
            'base/read/4',
            'base/read/5',
            'base/read/6',
            'base/read/child/'
        }, $paths);

        $this->assertEquals(Vector {}, $this->temp->read());
    }

    public function testReadSortingRecursive(): void {
        $this->vfs()->createStructure([
            'base/read' => [
                '4' => 'd',
                '5' => 'e',
                '6' => 'f',
                'child' => [
                    '1' => 'a',
                    '2' => 'b',
                    '3' => 'c'
                ],
                '1' => 'a',
                '2' => 'b',
                '3' => 'c',
            ]
        ]);

        $folder = new Folder($this->vfs()->path('base/read'));
        $contents = $folder->read(true, true);

        $scheme = $this->vfs()->scheme();
        $paths = $contents->map(($value) ==> {
            return str_replace($scheme . '://', '', $value->path());
        });

        $this->assertEquals(10, count($contents));
        $this->assertEquals(Vector {
            'base/read/1',
            'base/read/2',
            'base/read/3',
            'base/read/4',
            'base/read/5',
            'base/read/6',
            'base/read/child/',
            'base/read/child/1',
            'base/read/child/2',
            'base/read/child/3',
        }, $paths);

        $this->assertEquals(Vector {}, $this->temp->read());
    }

    public function testRename(): void {
        $file = new Folder($this->vfs()->path('base/rename'), true);

        $this->assertEquals($this->vfs()->path('base/rename/'), $file->path());
        $this->assertFileExists($this->vfs()->path('base/rename'));
        $this->assertFileNotExists($this->vfs()->path('base/rename-to'));

        $file->rename('rename-to');

        $this->assertEquals($this->vfs()->path('base/rename-to/'), $file->path());
        $this->assertFileNotExists($this->vfs()->path('base/rename'));
        $this->assertFileExists($this->vfs()->path('base/rename-to'));
    }

    public function testRenameMissingFile(): void {
        $this->assertFalse($this->temp->rename('temp-rename-to'));
    }

    public function testRenameExitsEarlySameLocation(): void {
        $file = new Folder($this->vfs()->path('base/rename'), true);

        $this->assertTrue($file->rename('rename'));
    }

    /**
     * @expectedException \Titon\Io\Exception\ExistingFileException
     */
    public function testRenameTargetExists(): void {
        $this->vfs()->createStructure([
            'base/rename' => [
                '1' => 'foo',
                '2' => 'foo'
            ],
            'base/rename-to' => [
                '3' => 'bar'
            ]
        ]);

        $folder = new Folder($this->vfs()->path('base/rename'));
        $folder->rename('rename-to', false);
    }

    public function testRenameOverwriteTargetFile(): void {
        $this->vfs()->createStructure([
            'base/rename' => [
                '1' => 'foo'
            ],
            'base/rename-to' => 'bar'
        ]);

        $file = new Folder($this->vfs()->path('base/rename'));

        $this->assertFileExists($this->vfs()->path('base/rename'));
        $this->assertFileExists($this->vfs()->path('base/rename-to'));
        $this->assertTrue(is_file($this->vfs()->path('base/rename-to')));

        $file->rename('rename-to', true);

        $this->assertFileNotExists($this->vfs()->path('base/rename'));
        $this->assertFileExists($this->vfs()->path('base/rename-to'));
        $this->assertFalse(is_file($this->vfs()->path('base/rename-to')));
    }

    public function testRenameOverwriteTargetFolder(): void {
        $this->vfs()->createStructure([
            'base/rename' => [
                '1' => 'foo'
            ],
            'base/rename-to' => [
                '2' => 'bar'
            ]
        ]);

        $file = new Folder($this->vfs()->path('base/rename'));

        $this->assertFileExists($this->vfs()->path('base/rename'));
        $this->assertFileExists($this->vfs()->path('base/rename-to'));
        $this->assertFileExists($this->vfs()->path('base/rename-to/2'));
        $this->assertTrue(is_dir($this->vfs()->path('base/rename-to')));

        $file->rename('rename-to', true);

        $this->assertFileNotExists($this->vfs()->path('base/rename'));
        $this->assertFileExists($this->vfs()->path('base/rename-to'));
        $this->assertFileNotExists($this->vfs()->path('base/rename/2'));
        $this->assertTrue(is_dir($this->vfs()->path('base/rename-to')));
    }

    public function testRenameFormatName(): void {
        $file = new Folder($this->vfs()->path('base/rename'), true);

        $this->assertTrue($file->rename('SA# DM)8NS #$ 97py1n  6 d6as9 #%@ P'));
        $this->assertEquals('SA--DM-8NS----97py1n--6-d6as9-----P', $file->name());
    }

    public function testReset(): void {
        $this->object->reset($this->vfs()->path('reset-to'));
        $this->assertEquals($this->vfs()->path('reset-to/'), $this->object->path());
    }

    /**
     * @expectedException \Titon\Io\Exception\InvalidPathException
     */
    public function testResetFailsOnFile(): void {
        $this->vfs()->createFile('base/reset-to', 'foo');

        $this->object->reset($this->vfs()->path('base/reset-to'));
    }

    public function testSize(): void {
        $this->vfs()->createStructure([
            'base/size' => [
                '1' => 'a',
                '2' => 'b',
                '3' => [],
                '4' => 'c',
                '5' => [
                    '1' => 'a'
                ]
            ]
        ]);

        $folder = new Folder($this->vfs()->path('base/size'));

        $this->assertEquals(5, $folder->size());
        $this->assertEquals(null, $this->temp->size());
    }

}
