Instructions
============

Instructions, also called gates, are used to represent the operations that must
be performed in parallel or in sequence to form the desired algorithm. In cQASM
1.x, they follow an assembly-like syntax:

.. code:: text

    <name> <comma-separated-operands>

Note the lack of parentheses around the operands.

.. note::

    libqasm does not specify an exact instruction set or semantics for
    instructions. This is to provide compatibility with the configurable
    instruction set in OpenQL. However, for many of cQASM's use cases, it makes
    sense to have a standardized instruction set (historically, this instruction
    set was in fact baked into the language). This standardized instruction set
    is specified at the bottom of this section.

Assignment instruction
----------------------

Special syntax is available for the ``set`` instruction, namely:

.. code:: text

    set <expr> = <expr>

This intends to make variable assignments more readable.

.. note::

    In cQASM 1.0 and 1.1, ``set`` behaves like a normal instruction with two
    operands. That means it must be defined as part of the instruction set via
    the API prior to parsing if the target supports it. In cQASM 1.2+, ``set``
    is a special instruction that requires no definition; cQASM 1.2+ readers
    are expected to support set instructions in all cases.

Goto instruction (1.2+)
-----------------------

In cQASM 1.2+, unstructured control-flow can be represented using
(non-repeating) subcircuits as labels and goto instructions. The syntax for a
``goto`` instruction is:

.. code:: text

    goto <name>

``<name>`` must match the name of exactly one subcircuit header in the program.

.. note::

    ``goto`` is a special instruction that bypasses the type system in order to
    correctly resolve the subcircuit name. It is always defined, regardless of
    whether a ``goto`` instruction is added to the instruction set via the API.
    In file versions older than 1.2, ``goto`` behaves like a normal
    instruction.

.. note::

    Conditional branches can be represented using the notation for conditional
    execution as defined in the next section.

Conditional instructions
------------------------

Instructions can be made conditional; that is, the instruction should only
execute if a certain classical boolean condition evaluates to true at runtime.
This can be used to alter the behavior of an algorithm based on previous
measurement results, for example to apply error correction. An instruction can
be made conditional in two ways:

.. code:: text

    cond (<condition>) <name> <operands>
    c-<name> <condition>, <operands>

Both forms are equivalent. The latter mostly exists for backward-compatibility,
as the former is considered more readable.

Whenever multiple measurement bits in the ``b`` register are selected for a
condition simultaneously, the states of these bits must be implicitly combined
using an AND gate at runtime.

.. note::

    You cannot specify multiple condition bits by comma-separating the bits.
    You have to use the slice notation on the bit register accordingly if you
    want this. For example,

    .. code:: text

        # c-x b[0], b[1], q[2]      <- not legal!
        c-x b[0,1], q[2]          # <- correct variant.

.. note::

    While syntactically identical, specification of multiple condition bits for
    a gate is completely unrelated to the single-gate-multiple-qubit notation
    described in the next subsection. Do not confuse the two! The reason for
    this oddity is, as usual, a historical one.

.. note::

    Selecting multiple bits at once is only supported within the condition of a
    gate, or as a gate operand via single-gate-multiple-qubit notation. It is
    illegal in all other contexts.

Single-gate-multiple-qubit
--------------------------

To more ergonomically specify many of the same gate operating in parallel on
multiple qubits, each qubit argument of a gate may be given more than one qubit
through the slice notation for the ``q`` register. The individual qubits are
then broadcast to individual gates. For example,

.. code:: text

    x q[0:3]
    # is functionally equivalent to
    x q[0] | x q[1] | x q[2] | x q[3]

For multi-qubit gates, the slice sizes for each qubit operand must match;

.. code:: text

    # cnot q[0], q[1,2] <- not legal, q[0] is not broadcast

Note that this never makes any sense for qubits, as a qubit can only be used
once within a group of parallel gates, and single-gate-multiple-qubit notation
asserts that the gates start simultaneously.

.. warning::

    The original libqasm API asserts uniqueness of the qubits in the slices by
    sorting the list of indices and silently removing duplicates. This is very
    much a bug:

    .. code:: text

        cnot q[1,2], q[3, 0]
        # becomes equivalent to...
        cnot q[1], q[0] | cnot q[2], q[3]
        # in the old API, but becomes
        cnot q[1], q[3] | cnot q[2], q[0]
        # in the new API (as you should expect)!

    This behavior was kept in place in the original libqasm API for backward
    compatibility, but the new API doesn't do the sort. In general, it is best
    to avoid single-gate-multiple-qubit notation for multi-qubit gates.

Default instruction set
-----------------------

Whenever libqasm's original API is used or the new API is instructed to use the
default instruction set, libqasm determines the supported set of instructions
and (implicitly) their semantics and, to some extent, timing behavior. The
instruction set is as follows.

``x <qubit>``
~~~~~~~~~~~~~

The Pauli-X gate is a single-qubit rotation through π radians around the X-axis.

``y <qubit>``
~~~~~~~~~~~~~

The Pauli-Y gate is a single-qubit rotation through π radians around the Y-axis.

``z <qubit>``
~~~~~~~~~~~~~

The Pauli-Z gate is a single-qubit rotation through π radians around the Z-axis.

``i <qubit>``
~~~~~~~~~~~~~

The identity gate leaves the state of a qubit unchanged. It thus acts as a
no-operation gate, which may be useful for certain simulation error models.

``h <qubit>``
~~~~~~~~~~~~~

The Hadamard gate is used to create a superposition of the two basis states.

``x90 <qubit>``
~~~~~~~~~~~~~~~

The x90 gate is a single-qubit rotation through 1/2 π radians around the X-axis.

``mx90 <qubit>``
~~~~~~~~~~~~~~~~

The mx90 gate is a single-qubit rotation through negative 1/2 π radians around
the X-axis.

``y90 <qubit>``
~~~~~~~~~~~~~~~

The y90 gate is a single-qubit rotation through 1/2 π radians around the Y-axis.

``my90 <qubit>``
~~~~~~~~~~~~~~~~

The my90 gate is a single-qubit rotation through negative 1/2 π radians around
the Y-axis.

``s <qubit>``
~~~~~~~~~~~~~

The S gate is a single-qubit rotation through 1/2 π radians around the Z-axis.

``sdag <qubit>``
~~~~~~~~~~~~~~~~

The S-dagger gate is a single-qubit rotation through negative 1/2 π radians
around the Z-axis.

``t <qubit>``
~~~~~~~~~~~~~

The T gate is a single-qubit rotation through 1/4 π radians around the Z-axis.

``tdag <qubit>``
~~~~~~~~~~~~~~~~

The T-dagger gate is a single-qubit rotation through negative 1/4 π radians
around the Z-axis.

``rx <qubit>, <angle>``
~~~~~~~~~~~~~~~~~~~~~~~

Performs an arbitrary rotation around the X axis on the given qubit. The angle
is specified in radians.

``ry <qubit>, <angle>``
~~~~~~~~~~~~~~~~~~~~~~~

Performs an arbitrary rotation around the Y axis on the given qubit. The angle
is specified in radians.

``rz <qubit>, <angle>``
~~~~~~~~~~~~~~~~~~~~~~~

Performs an arbitrary rotation around the Z axis on the given qubit. The angle
is specified in radians.

``u <qubit>, <matrix>``
~~~~~~~~~~~~~~~~~~~~~~~

The U gate applies an arbitrary single-qubit unitary gate to the given qubit.
The matrix must be a complex 2-by-2 unitary matrix, though libqasm does not
assert the unitary condition of the matrix.

``cnot <qubit>, <qubit>``
~~~~~~~~~~~~~~~~~~~~~~~~~

Applies a CNOT (controlled X) gate on the given qubits. The first qubit is the
control, the second qubit is the target.

``cz <qubit>, <qubit>``
~~~~~~~~~~~~~~~~~~~~~~~

Applies a controlled phase (controlled Z) gate on the given qubit pair.

``swap <qubit>, <qubit>``
~~~~~~~~~~~~~~~~~~~~~~~~~

Swaps the state of the given two qubits.

``cr <qubit>, <qubit>, <angle>``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Applies a controlled phase (controlled Z) gate with the given rotation angle in
radians on the given qubit pair. The first qubit is the control qubit, the
second is the target.

``crk <qubit>, <qubit>, <k>``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Applies a controlled phase (controlled Z) gate with the given rotation angle on
the given qubit pair. The rotation angle is π/2\ :sup:`k` radians. The first
qubit is the control qubit, the second is the target.

``toffoli <qubit>, <qubit>, <qubit>``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Applies a Toffoli gate (controlled X with two control qubits) on the given
qubits. The first two qubits are the control qubits, the third is the target.

``prep <qubit>``
~~~~~~~~~~~~~~~~

Prepares the given qubit in the Z basis (\|0>). Synomym for ``prep_z``.

``prep_x <qubit>``
~~~~~~~~~~~~~~~~~~

Prepares the given qubit in the X basis.

``prep_y <qubit>``
~~~~~~~~~~~~~~~~~~

Prepares the given qubit in the Y basis.

``prep_z <qubit>``
~~~~~~~~~~~~~~~~~~

Prepares the given qubit in the Z basis (\|0>). Synomym for ``prep``.

``measure <qubit>``
~~~~~~~~~~~~~~~~~~~

Measures the given qubit in the Z basis. \|0> results in false, \|1> results in
true. If the qubit is part of the qubit register, the measurement is stored in
the accompanying measurement bit; if it is a variable, the result is discarded.
Synonym for ``measure_z``.

``measure_x <qubit>``
~~~~~~~~~~~~~~~~~~~~~

Measures the given qubit in the X basis. If the qubit is part of the qubit
register, the measurement is stored in the accompanying measurement bit; if it
is a variable, the result is discarded.

``measure_y <qubit>``
~~~~~~~~~~~~~~~~~~~~~

Measures the given qubit in the Y basis. If the qubit is part of the qubit
register, the measurement is stored in the accompanying measurement bit; if it
is a variable, the result is discarded.

``measure_z <qubit>``
~~~~~~~~~~~~~~~~~~~~~

Measures the given qubit in the Z basis. \|0> results in false, \|1> results in
true. If the qubit is part of the qubit register, the measurement is stored in
the accompanying measurement bit; if it is a variable, the result is discarded.
Synonym for ``measure``.

``measure_all``
~~~~~~~~~~~~~~~

Measures all qubits in the qubit register in the Z basis, and stores the results
in the measurement bit register. \|0> results in false, \|1> results in true.
This instruction cannot share a bundle with other instructions.

``measure_parity <qubit>, <axis>, <qubit>, <axis>``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Refer to section IV-A of the `arXiv paper <https://arxiv.org/pdf/1805.09607v1.pdf>`_.

``skip <integer>``
~~~~~~~~~~~~~~~~~~

Skip the specified number of cycles. The bundle following the skip will start
the given amount plus one cycle after the bundle preceding the skip. For
example:

.. code:: text

    x q[0]  # starts in cycle i
    skip 3  # starts in cycle i+1
    x q[0]  # starts in cycle i+4

This instruction cannot share a bundle with other instructions.

``wait <qubit>, <integer>``
~~~~~~~~~~~~~~~~~~~~~~~~~~~

For each of the specified qubits, wait independently for all previous instructions
involving the specified qubit to finish, and then wait at least the given number
of cycles before starting a next instruction involving the specified qubit.
In essence, this acts as a no-op instruction for the specified qubit with the
specified duration in cycles. The individual waits on each of the qubits
are independent of each other, following the regular single-gate-multiple-qubit
rules.

.. note::

    When OpenQL is used, you should use ``wait`` instructions rather than
    ``skip`` to introduce delays. The scheduler will ignore any other timing
    semantics in your program, including whether you placed instructions in a
    bundle or not. The timing of the algorithm after scheduling will be
    represented using skip instructions and bundles exclusively. Nevertheless,
    the ``wait`` instructions will remain, so the requested (minimum) delays
    are also represented, so running the scheduler again would not break the
    program. For example,

    .. code:: text

        x q[0]
        wait q[0], 3
        z q[0]

    may compile into

    .. code:: text

        x q[0]       # starts in cycle 0
        wait q[0],3  # starts in cycle 1
        skip 2       # starts in cycle 2
        z q[0]       # starts in cycle 4

``barrier <qubit>``
~~~~~~~~~~~~~~~~~~~

Waits for all operations on all given qubit(s) to finish, before advancing to the
next instruction. For the qubit argument of barrier, single-gate multiple-qubit notation
is used.

.. note::

    While the single-gate-multiple-qubit notation is used to specify multiple qubits
    involved in the barrier gate, the individual qubits should not broadcast to
    individual barrier gates. For example:

    .. code:: text

        barrier q[0:3]
        # should not broadcast to
        barrier q[0] | barrier q[1] | barrier q[2] | barrier q[3]

``not <bit-ref>``
~~~~~~~~~~~~~~~~~

Inverts the state of the given measurement bit register or classical bit
variable.

``display [bit-ref]``
~~~~~~~~~~~~~~~~~~~~~

Meta-instruction for simulators, telling the simulator to print the bit and
qubit state for the given bit reference, or for all qubits if the optional
reference is omitted. This instruction cannot share a bundle with other
instructions.

``display_binary [bit-ref]``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Meta-instruction for simulators, telling the simulator to print the given
measurement bit state, or the state of all measurement bits in the register if
the optional reference is omitted. This instruction cannot share a bundle with
other instructions.

``reset-averaging [qubit]``
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Meta-instruction for simulators, telling the simulator to reset internal
averaging counters for all measurements performed up to that point for the given
qubit(s), or all qubits in the register if no reference is specified. This
instruction cannot share a bundle with other instructions.

.. admonition:: Historical

    This instruction uses a dash in the name instead of an underscore for some
    reason, requiring a special case in the tokenizer (after all, it would be
    a subtraction otherwise). It is not possible to define custom instructions
    with dashes in them other than exactly ``reset-averaging``.

``load_state <file_name>``
~~~~~~~~~~~~~~~~~~~~~~~~~

Meta-instruction for simulators, telling the simulator to load the qubit
register state from the given file_name. The file_name is to be specified as a
string literal. This instruction cannot share a bundle with other instructions.



