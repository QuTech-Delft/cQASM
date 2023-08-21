
# parsetab.py
# This file is automatically generated. Do not edit.
# pylint: disable=W,C,R
_tabversion = '3.10'

_lr_method = 'LALR'

_lr_signature = "BREAK COND CONTINUE ELSE FLOAT_LITERAL FOR IDENTIFIER IF INT_LITERAL MAP NEWLINE OPERATOR Q QUBITS SET THEN VAR VERSION WHILEProgram : OptNewline VERSION Newline QUBITS Newline StatementList OptNewlineNewline : Newline NEWLINE\n                | NEWLINEOptNewline : Newline\n                  | EmptyEmpty :StatementList : StatementList Newline StatementStatementList : StatementStatement : Subcircuit\n                 | Map\n                 | GateSubcircuit : '.' IDENTIFIERSubcircuit : '.' IDENTIFIER '(' INT_LITERAL ')' Argument : Q '[' INT_LITERAL ':' INT_LITERAL ']' Argument : Q '[' INT_LITERAL ']' Argument :   INT_LITERAL\n                  | FLOAT_LITERALArgument :   IDENTIFIERGateApplication : GateApplication ',' ArgumentGateApplication : IDENTIFIER ArgumentGate : GateApplicationMap : MAP Q '[' INT_LITERAL ']' ',' IDENTIFIER "
    
_lr_action_items = {'NEWLINE':([0,3,5,6,7,8,9,10,11,12,13,14,15,19,21,22,23,24,26,27,30,34,38,40,44,45,],[5,7,-3,5,-2,7,5,7,5,-8,-9,-10,-11,-21,7,-12,-18,-20,-16,-17,-7,-19,-13,-15,-14,-22,]),'VERSION':([0,2,3,4,5,7,],[-6,6,-4,-5,-3,-2,]),'$end':([1,4,5,7,11,12,13,14,15,19,20,21,22,23,24,26,27,30,34,38,40,44,45,],[0,-5,-3,-2,-6,-8,-9,-10,-11,-21,-1,-4,-12,-18,-20,-16,-17,-7,-19,-13,-15,-14,-22,]),'QUBITS':([5,7,8,],[-3,-2,9,]),'.':([5,7,10,21,],[-3,-2,16,16,]),'MAP':([5,7,10,21,],[-3,-2,18,18,]),'IDENTIFIER':([5,7,10,16,17,21,29,43,],[-3,-2,17,22,23,17,23,45,]),'Q':([17,18,29,],[25,28,25,]),'INT_LITERAL':([17,29,31,32,33,39,],[26,26,35,36,37,42,]),'FLOAT_LITERAL':([17,29,],[27,27,]),',':([19,23,24,26,27,34,40,41,44,],[29,-18,-20,-16,-17,-19,-15,43,-14,]),'(':([22,],[31,]),'[':([25,28,],[32,33,]),')':([35,],[38,]),':':([36,],[39,]),']':([36,37,42,],[40,41,44,]),}

_lr_action = {}
for _k, _v in _lr_action_items.items():
   for _x,_y in zip(_v[0],_v[1]):
      if not _x in _lr_action:  _lr_action[_x] = {}
      _lr_action[_x][_k] = _y
del _lr_action_items

_lr_goto_items = {'Program':([0,],[1,]),'OptNewline':([0,11,],[2,20,]),'Newline':([0,6,9,11,],[3,8,10,21,]),'Empty':([0,11,],[4,4,]),'StatementList':([10,],[11,]),'Statement':([10,21,],[12,30,]),'Subcircuit':([10,21,],[13,13,]),'Map':([10,21,],[14,14,]),'Gate':([10,21,],[15,15,]),'GateApplication':([10,21,],[19,19,]),'Argument':([17,29,],[24,34,]),}

_lr_goto = {}
for _k, _v in _lr_goto_items.items():
   for _x, _y in zip(_v[0], _v[1]):
       if not _x in _lr_goto: _lr_goto[_x] = {}
       _lr_goto[_x][_k] = _y
del _lr_goto_items
_lr_productions = [
  ("S' -> Program","S'",1,None,None,None),
  ('Program -> OptNewline VERSION Newline QUBITS Newline StatementList OptNewline','Program',7,'p_program','CQASMParser.py',210),
  ('Newline -> Newline NEWLINE','Newline',2,'p_newline','CQASMParser.py',231),
  ('Newline -> NEWLINE','Newline',1,'p_newline','CQASMParser.py',232),
  ('OptNewline -> Newline','OptNewline',1,'p_optnewline','CQASMParser.py',236),
  ('OptNewline -> Empty','OptNewline',1,'p_optnewline','CQASMParser.py',237),
  ('Empty -> <empty>','Empty',0,'p_empty','CQASMParser.py',241),
  ('StatementList -> StatementList Newline Statement','StatementList',3,'p_statementlist_add','CQASMParser.py',245),
  ('StatementList -> Statement','StatementList',1,'p_statementlist_single','CQASMParser.py',249),
  ('Statement -> Subcircuit','Statement',1,'p_statement','CQASMParser.py',253),
  ('Statement -> Map','Statement',1,'p_statement','CQASMParser.py',254),
  ('Statement -> Gate','Statement',1,'p_statement','CQASMParser.py',255),
  ('Subcircuit -> . IDENTIFIER','Subcircuit',2,'p_subcircuit','CQASMParser.py',259),
  ('Subcircuit -> . IDENTIFIER ( INT_LITERAL )','Subcircuit',5,'p_subcircuit_iterations','CQASMParser.py',263),
  ('Argument -> Q [ INT_LITERAL : INT_LITERAL ]','Argument',6,'p_argument_qubits','CQASMParser.py',267),
  ('Argument -> Q [ INT_LITERAL ]','Argument',4,'p_argument_qubit','CQASMParser.py',271),
  ('Argument -> INT_LITERAL','Argument',1,'p_argument_literal','CQASMParser.py',275),
  ('Argument -> FLOAT_LITERAL','Argument',1,'p_argument_literal','CQASMParser.py',276),
  ('Argument -> IDENTIFIER','Argument',1,'p_argument_variable','CQASMParser.py',280),
  ('GateApplication -> GateApplication , Argument','GateApplication',3,'p_gateApplication_add','CQASMParser.py',284),
  ('GateApplication -> IDENTIFIER Argument','GateApplication',2,'p_gateApplication_single','CQASMParser.py',288),
  ('Gate -> GateApplication','Gate',1,'p_instruction_gate','CQASMParser.py',292),
  ('Map -> MAP Q [ INT_LITERAL ] , IDENTIFIER','Map',7,'p_map','CQASMParser.py',296),
]
