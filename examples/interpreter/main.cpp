#include "../utils.hpp"

#include <cstdio>
#include <iostream>
#include <sstream>  // ostringstream
#include <stdexcept>

// Include the generated files.
#include "value.hpp"
#include "program.hpp"


// Note: the // comment contents of main(), together with the MARKER lines and
// the output of the program, are used to automatically turn this into a
// restructured-text page for ReadTheDocs.

int main() {
    // *******************
    // Interpreter example
    // *******************
    //
    // This example discusses some of the more advanced features of tree-gen,
    // using an interpreter for a very simple language as an example. This is,
    // however, not a completely integrated or even functional interpreter; we
    // just go over some of the concepts and things you may run into when you
    // would make such an interpreter.
    //
    // External nodes & multiple trees
    // ===============================
    //
    // When trees get large and complicated, you may want to split a tree up
    // into multiple files. This is supported within tree-gen, as long as there
    // are no circular references to nodes between the files. In this example,
    // we've split up the expression-like nodes into a tree separate from the
    // statement-like nodes. The former, ``value.tree``, has nothing special
    // going on in this regard, while the latter, ``program.tree``, effectively
    // includes the former. The files are listed at the bottom of this page
    // for your convenience.
    //
    // tree-gen itself is unaware of any links between files. For
    // ``program.tree``, it simply generates code assuming that the types
    // referred to through the ``external`` edges exist. For this to be true,
    // you have to tell tree-gen to include the external tree's generated
    // header file at the top of its generated header file using the ``include``
    // directive. Note also that, as tree-gen is unaware of the external tree,
    // you'll have to use the full C++ TitleCase type name, rather than the
    // snake_case name for normal edges.
    //
    // Integration with Flex/Bison (or other parsers)
    // ==============================================
    //
    // When making an interpreter (or parser of any kind) with tree-gen, you'll
    // probably want to use a tokenizer/parser generator such as Flex/Bison.
    // With Bison at least, you'll quickly run into a fundamental issue when
    // trying to store tree-gen nodes in the generated ``yyval`` union:
    // union in C++ can only consist of types with trivial constructors, and
    // tree-gen nodes are not among those. The author is unfamiliar with
    // other frameworks like it, but this will probably apply for any C-style
    // parser generator.
    //
    // The solution is to use raw pointers to the generated node types in this
    // enumeration. For instance, you may end up with the following (which would
    // be generated by Bison):
    union node_type {
        program::Program *prog;
        program::Assignment *asgn;
        value::Literal *lit;
        value::Reference *ref;
        // ...
    };
    MARKER

    // To create a new node, instead of using ``tree::base::make``,
    // you use the ``new`` keyword directly.
    node_type n1{};
    node_type n2{};
    n1.lit = new value::Literal(2);
    n2.ref = new value::Reference("x");
    MARKER

    // The generated constructors won't help you much for non-leaf nodes,
    // because they expect edge wrappers (One, Maybe, etc.) around the
    // parameters, so you'll typically want to use the constructor without
    // arguments. Instead, to assign child nodes, you can use
    // ``Maybe::set_raw()``, ``One::set_raw()``, ``Any::add_raw()``,
    // or ``Many::add_raw()``. These functions take ownership of a new-allocated
    // raw pointer to a node - exactly what we need here. Note that because they
    // take ownership, you don't have to (and shouldn't!) delete manually.
    node_type n3{};
    n3.asgn = new program::Assignment();
    n3.asgn->rhs.set_raw(n1.lit);
    n3.asgn->lhs.set_raw(n2.ref);

    node_type n4{};
    n4.prog = new program::Program();
    n4.prog->statements.add_raw(n3.asgn);
    MARKER

    // Bison will ultimately pass you the final parsed node, which you then have
    // to put into a ``One`` edge yourself:
    auto tree = tree::base::One<program::Program>();
    tree.set_raw(n4.prog);
    tree->dump();
    MARKER

    // Note that when you're using Bison, the enum and its types is largely
    // hidden from you. In the actions the above would look more like this:
    //
    // .. code-block:: none
    //
    //   // Action for a literal:
    //   { $$ = new value::Literal(std::stoi($1)); }
    //
    //   // Action for a reference:
    //   { $$ = new value::Reference($1); /* and deallocate $1, assuming it's a char* */ }
    //
    //   // Action for a assignment:
    //   { $$ = new value::Assignment(); $$->lhs.set_raw($1); $$->rhs.set_raw($3); }
    //
    // and so on.
    //
    // Syntax error recovery
    // ---------------------
    //
    // Some parser generators (like Bison) allow you to specify recovery rules
    // in case of a syntax error, so the parser doesn't just die immediately.
    // This is necessary to emit more than a single error message at a time.
    // To help you store recovery information in the tree for post-mortem
    // analysis, tree-gen has a special ``error`` directive that you can place
    // inside a node class. For example, if you make a recovery rule that
    // assumes any semicolon encountered after a syntax error produces a broken
    // statement, you can make a node like this:
    //
    // .. code-block:: none
    //
    //   # An erroneous statement.
    //   erroneous_statement {
    //       error;
    //   }
    //
    // Such nodes behave exactly like any other node, with one exception: they
    // always indicate that they're not well-formed.
    auto err_stmt = tree::base::make<program::ErroneousStatement>();
    ASSERT_RAISES(tree::base::NotWellFormed, err_stmt.check_well_formed());
    MARKER

    // Line number information (and annotations in general)
    // ----------------------------------------------------
    //
    // When you're doing any kind of serious parsing, you'll want to store as
    // much line number and other contextual information as possible, to make
    // your error messages as clear as possible. You could of course add this
    // information to every node in the tree specification file, perhaps using
    // inheritance to prevent excessive repetition, but this gets annoying fast,
    // especially when it comes to tree-gen's well-formedness rules.
    //
    // Instead, you should be using annotations for this. Annotations are
    // objects added to nodes without you ever having to declare that you're
    // going to add them. If that sounds like black magic to you in the context
    // of C++, well, that's because it is: internally annotations are stored
    // as a map from ``std::type_index`` to a C++11 backport of ``std::any``...
    // it's complicated. But you don't have to worry about it. What matters is
    // that each and every node generated by tree-gen (or anything else that
    // inherits from ``tree::annotatable::Annotatable``) can have zero or one
    // annotation of every C++ type attached to it.
    //
    // The use of annotations is not limited to metadata (although that is its
    // primary purpose); they also allow bits of code to temporarily attach
    // their own data to tree nodes, without the tree definitions needing to be
    // updated to reflect this. This is especially useful when you're using
    // tree-gen in a library, and you want to let users operate on your trees.
    // After all, they wouldn't be able to modify the tree definition file
    // without forking and recompiling your library.
    //
    // tree-gen has only one special case for annotations, intended for adding
    // line number information to debug dumps. The ``location`` directive is
    // used for this:
    //
    // .. code-block:: none
    //
    //   // Source location annotation type. The generated dumper will see if this
    //   // annotation exists for a node, and if so, write it to the debug dump using
    //   // the << stream operator.
    //   location primitives::SourceLocation
    //
    // Without going into too much detail about the Annotatable interface (just
    // look at the API docs for that), here's an example of how it would work.
    n1.lit->set_annotation(primitives::SourceLocation("test", 1, 5));
    n2.ref->set_annotation(primitives::SourceLocation("test", 1, 1));
    n3.asgn->set_annotation(primitives::SourceLocation("test", 1, 1));
    n4.prog->set_annotation(primitives::SourceLocation("test", 1, 1));
    tree->dump();
    MARKER

    // Testing the JSON dump
    std::ostringstream oss{};
    tree->dump_json(oss);
    ASSERT(oss.str() ==
        R"({"Program":{"variables":"[]","statements":[{"Assignment":{"lhs":{"Reference":{"n)"
        R"(ame":"x","target":"!MISSING","source_location":"test:1:1"}},"rhs":{"Literal":{"v)"
        R"(alue":"2","source_location":"test:1:5"}},"source_location":"test:1:1"}}],"source)"
        R"(_location":"test:1:1"}})"
    );
    std::printf("%s", oss.str().c_str());
    std::printf("\n");
    MARKER

    // Note that we can still use these pointers despite ownership having been
    // passed to the node objects because they refer to the same piece of
    // memory. In practice, though, you would do this during the parser actions,
    // just after constructing them.
    //
    // When serializing and deserializing, annotations are ignored by default;
    // they can be of any type whatsoever, and C++ can't dynamically introspect
    // which types can be (de)serialized and which can't be, after all. So even
    // though the example SourceLocation object extends
    // ``tree::annotatable::Serializable``, this doesn't work automagically.
    auto node = tree::base::make<value::Literal>(2);
    node->set_annotation(primitives::SourceLocation("test", 1, 1));
    std::string cbor = tree::base::serialize(tree::base::Maybe<value::Literal>{ node.get_ptr() });
    auto node2 = tree::base::deserialize<value::Literal>(cbor);
    ASSERT(!node2->has_annotation<primitives::SourceLocation>());

    // However, you *can* register annotations types for serialization and
    // deserialization if you want to through the
    // ``tree::annotatable::serdes_registry`` singleton. After that, it will
    // work.
    tree::annotatable::serdes_registry.add<primitives::SourceLocation>("loc");
    cbor = tree::base::serialize(tree::base::Maybe<value::Literal>{ node.get_ptr() });
    node2 = tree::base::deserialize<value::Literal>(cbor);
    ASSERT(node2->has_annotation<primitives::SourceLocation>());
    MARKER

    // Two ``add()`` methods are provided. The one used here assumes that the
    // type has an appropriate ``serialize()`` method and an appropriate
    // constructor for deserialization, the other allows you to specify them
    // manually using ``std::function``s. The name is optional but strongly
    // recommended; if not used, a C++-compiler-specific identifier will be used
    // for the type.
    //
    // Similar to links, annotations aren't copied as you might expect by
    // ``copy()`` and ``clone()``. Specifically, annotations are stored as
    // ``std::shared_ptr``s to unknown C++ objects, and therefore copying a
    // node only copies the references to the annotations. To fully clone
    // annotations, you'll either have to serialize and deserialize the node
    // they belong to (after registering with ``serdes_registry`` of course), or
    // clone them manually.

    // Visitor pattern
    // ===============
    //
    // In the directory example, we avoided the difficulty of dealing with edges
    // to incomplete types, such as a ``One<value::Rvalue>``, aside from a
    // single ``as_file()`` call halfway through. Spamming such typecasts to,
    // for instance, evaluate an rvalue expression during constant propagation
    // or interpreting, is not very scalable. It can also lead to headache down
    // the line if you ever have to add more subclasses, as it's easy to forget
    // or not bother to check for unknown types when initially writing that code
    // that way. tree-gen also generates the necessary classes for the visitor
    // pattern for you to avoid this.
    //
    // In the visitor pattern, you define a class that implements the
    // appropriate visitor interface, and (in this case) the nodes will call
    // methods on this class depending on their type when you pass the visitor
    // object to them through their ``visit()`` method. Because the visitor
    // interface classes are also generated by tree-gen and have sane default
    // behavior, your code will "by default" be future-proof, in the sense that
    // you'll get an error if something unexpected happens, rather than it
    // failing silently and maybe crashing down the line.
    //
    // Two visitor base classes are provided, ``Visitor<T=void>`` and
    // ``RecursiveVisitor``. In either case, you have to override
    // ``visit_node()``, which is called when a node is encountered for which
    // no specialization is available; this should just throw an appropriate
    // exception. You can then optionally override any node in the node class
    // hierarchy to provide the actual behavior.
    //
    // The two classes differ in the default behavior when an unspecialized
    // node is encountered: ``Visitor`` always defers up the class hierarchy,
    // while ``RecursiveVisitor`` provides a default implementation for node
    // types with edges, where it simply recursively visits the child nodes
    // depth-first.
    //
    // The T type in ``Visitor<T>`` refers to the return type for the visit
    // methods. It's fixed to ``void`` for ``RecursiveVisitor``, because if a
    // node has multiple children, there is nothing sensible to return.
    //
    // Let's look at what an expression evaluator looks like for this.
    class RvalueEvaluator : public value::Visitor<int> {
    public:
        int visit_node(value::Node &node) override {
            throw std::runtime_error("unknown node type");
        }

        int visit_literal(value::Literal &node) override {
            return node.value;
        }

        int visit_negate(value::Negate &node) override {
            return -node.oper->visit(*this);
        }

        int visit_add(value::Add &node) override {
            return node.lhs->visit(*this) + node.rhs->visit(*this);
        }

        int visit_sub(value::Sub &node) override {
            return node.lhs->visit(*this) - node.rhs->visit(*this);
        }

        int visit_mul(value::Mul &node) override {
            return node.lhs->visit(*this) * node.rhs->visit(*this);
        }

        int visit_div(value::Div &node) override {
            return node.lhs->visit(*this) / node.rhs->visit(*this);
        }

        int visit_reference(value::Reference &node) override {
            return node.target->value;
        }
    };

    // 2 + (3 * 4) = 14
    auto expr = tree::base::make<value::Add>(
        tree::base::make<value::Literal>(2),
        tree::base::make<value::Mul>(
            tree::base::make<value::Literal>(3),
            tree::base::make<value::Literal>(4)
        )
    );
    RvalueEvaluator eval{};
    ASSERT(expr->visit(eval) == 14);
    MARKER

    // If we hadn't implemented ``visit_node()``, the
    // ``RvalueEvaluator eval{};`` line would break, as RvalueEvaluator would
    // be an abstract class because of it. And while the above may look pretty
    // complete, it is in fact not:
    ASSERT_RAISES(std::runtime_error, value::ErroneousValue().visit(eval));
    MARKER

    // The visitor pattern is more powerful than recursively calling functions
    // for other reasons as well, because they can be specialized through
    // inheritance, state can be maintained in the class as the tree is
    // traversed (the ``dump()`` method is actually implemented this way
    // internally), it can be written inline within a function despite the
    // recursive nature, and so on. But it's a lot more verbose than the
    // ``as_*()`` alternative for simple cases. The choice between visitor
    // pattern and casts is therefore usually a matter of personal preference.
    MARKER

    return 0;
}
