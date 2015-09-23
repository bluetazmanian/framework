```
 ______  ______   ______   __    __   ______   __    __   ______   ______   __  __
/\  ___\/\  __ \ /\  __ \ /\ '-./  \ /\  ___\ /\ \ __\ \ /\  __ \ /\  __ \ /\ \/ /_
\ \  __\\ \  __< \ \  __ \\ \ \._/\ \\ \  __\ \ \ \| \\ \\ \ \/\ \\ \  __< \ \  _  \
 \ \_\/  \ \_\ \_\\ \_\ \_\\ \_\/\ \_\\ \_____\\ \__/_.__\\ \_____\\ \_\ \_\\ \_\ \_\
  \/_/    \/_/\/_/ \/_/\/_/ \/_/  \/_/ \/_____/ \/_/  \/_/ \/_____/ \/_/\/_/ \/_/\/_/
```

# Titon Framework #
[![Project Titon](https://img.shields.io/badge/project-titon-82667d.svg?style=flat)](http://titon.io)
[![Build Status](https://img.shields.io/travis/titon/framework.svg?style=flat)](https://travis-ci.org/titon/framework)
[![HHVM](https://img.shields.io/hhvm/titon/framework.svg?style=flat)](https://github.com/titon/framework)
[![Latest Version](https://img.shields.io/packagist/v/titon/framework.svg?style=flat)](https://packagist.org/packages/titon/framework)
[![Total Downloads](https://img.shields.io/packagist/dm/titon/framework.svg?style=flat)](https://packagist.org/packages/titon/framework)
[![License](https://img.shields.io/packagist/l/titon/framework.svg?style=flat)](https://github.com/titon/framework/blob/master/license.md)

## What is Titon? ##

Titon is a modular web framework built on Hack that runs in HHVM. It is written with simplicity and efficiency in mind, with enough room for extensibility and abstraction. The framework takes advantage of Hack's strict mode, type system, generics, collections, and more. It also integrates the wonderful PSRs from PHP-FIG for increased interoperability with external PHP code bases and libraries.

Looking for more information? Find us in [#titon](http://webchat.freenode.net/?channels=titon) on FreeNode.

## Requirements ##

* [HHVM 3.8.0](http://hhvm.com/)
* [Hack](http://hacklang.org/)
* [Composer](http://getcomposer.org)

## Roadmap ##

Upcoming features and release plan.

* Q4 2015
    * Console package - Command line application [DELAYED]
    * HTTP package - Updated for PSR-7 support
    * [MVC package](https://github.com/titon/mvc) - Model-view-controller application [DELAYED UNTIL PSR-7]
    * Pipeline package - Piped command streams
* Q1 2016
    * CommandBus package - Command bus layer
    * Queue package - Queue, job, and worker services
    * Session package - Custom session handler
* Q2 2016
    * [DB package](https://github.com/titon/db) - Database abstraction layer
    * [Model package](https://github.com/titon/model) - Object relational mapper and active record for DB entities
* Q3 2016
    * Mail package - Emailing layer
    * ACL package - Access control lists
    * Auth package - Authentication and authorization
* Q4 2016
    * ADR package - Action-domain-responder application
    * WebSocket package - Web socket based application

Features and additions currently in discussion.

* Separate Type\Xml into its own package
* Rewrite the Type package into a data structure oriented package
* Implement a custom Yaml package that doesn't rely own Zend compat mode
* Possibly merge Utility and Common packages

## Usage ##

Documentation is currently in the works, but in the mean time, check out the following articles.

* [Installation](docs/en/setup/installing.md)
* [Packages](docs/en/packages/index.md)

## Contributing ##

Titon is accepting any knowledgeable PHP developers looking to get their hands and feet wet in Hack. Chat with us over in [#titon](http://webchat.freenode.net/?channels=titon) for more information.

## Testing ##

Titon comes packaged with a Vagrant box and a handful of Python scripts to ease the testing process. Boot up the Vagrant instance with `vagrant up` and SSH into the box with `vagrant ssh`. Once in Vagrant, you can run the following command to run tests.

```bash
/vagrant/bin/run-tests
```

Or run tests for a single file or folder.

```bash
/vagrant/bin/run-tests --path=Titon/Common
```

Or filter tests based on class or method name.

```bash
/vagrant/bin/run-tests --filter=methodName
```

#### Hack Strict Mode ####

For the most part, Titon strives to be strict compliant. This can be very challenging, but with Hack's built-in type checker, it couldn't be easier. Simply run the following command to check for strict issues.

```bash
/vagrant/bin/type-check
```

Or type check a specific folder.

```bash
/vagrant/bin/type-check --path=Titon/Utility
```

When filtering down by path, multiple errors will still arise like "Was expecting a class", "Unbound name", "Unbound global constant", "This is not a container, this is an object of type X", etc. The main cause of this issue is that the type checker is ran in a sub-folder, and not the root, so all those classes are not in scope. This should not be an issue if the type checker is ran in the source root.
