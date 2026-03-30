import { ASTNode } from "../lang-maker/ast";
import { SemanticsBuilder } from "../lang-maker/semantics";

export namespace qbscript {
    export type Type = NonNullable<qbscript.TypeIdentifier['name']>
    export type ValueType = qbscript.Value['value_type']

    export class Script extends ASTNode {
        comments: Comment[]
        macros: Macro[]
        declarations: Declaration[]
        statements: Statement[]
        children = () => [...this.comments, ...this.macros, ...this.declarations, ...this.statements]
    }
    export class Comment extends ASTNode {}

    // Macros
    export type Macro = UseDeviceMacro;
    export class UseDeviceMacro extends ASTNode {
        device: string
    }

    // Declarations
    export type Declaration = VariableDeclaration | PointerDeclaration

    export class VariableDeclaration extends ASTNode {
        identifier: Identifier
        value: Value | Reference
        children = () => [this.identifier, this.value]
    }
    export class PointerDeclaration extends ASTNode {
        identifier: Identifier
        ref: Reference
        value?: Value | Reference
        children = () => [this.identifier, this.ref, this.value]
    }

    // Statements
    export type Statement = HoldStatement | ReleaseStatement | LogStatement | SleepStatement | ReturnStatement | ResetStatement | RebootStatement
    export class AssignStatement extends ASTNode {
        target: Reference
        source: Value | Reference
        children = () => [this.target, this.source]
    }
    export class IfStatement extends ASTNode {
        expression: BoolExpression
        children = () => [this.expression]
    }
    export class ElseStatement extends ASTNode {}
    export class ElseIfStatement extends ASTNode {
        statement: IfStatement
        children = () => [this.statement]
    }
    export class WhileStatement extends ASTNode {
        expression: BoolExpression
        children = () => [this.expression]
    }
    export class HoldStatement extends ASTNode {
        device: string
    }
    export class ReleaseStatement extends ASTNode {
        device: string
    }
    export class LogStatement extends ASTNode {
        value: Value | Reference
        children = () => [this.value]
    }
    export class SleepStatement extends ASTNode {
        time: number
    }
    export class ReturnStatement extends ASTNode {
        value: Value | Reference
        children = () => [this.value]
    }
    export class ResetStatement extends ASTNode {}
    export class RebootStatement extends ASTNode {}

    // Expressions
    export class BoolExpression extends ASTNode {
        terms: BoolExpressionTerm[]
        mods: ('and'|'or')[]
        children = () => [...this.terms]
    }
    export class BoolExpressionTerm extends ASTNode {
        value: BoolOperation | BoolExpression
        children = () => [this.value]
    }
    export class BoolOperation extends ASTNode {
        left: Value | Reference
        op: '==' | '!=' | '>' | '<' | '>=' | '<='
        right: Value | Reference
        children = () => [this.left, this.right]
    }

    export class MathExpression extends ASTNode {
        terms: MathExpressionTerm[]
        ops: ('+'|'-'|'*'|'/'|'%'|'^')[]
        children = () => [...this.terms]
    }
    export class MathExpressionTerm extends ASTNode {
        value: Value | MathExpression
        children = () => [this.value]
    }

    // References
    export class Reference extends ASTNode {
        device?: string
        node: string
        index?: number | Reference
        ref!: Identifier
    }
    
    // Identifiers

    export class TypeIdentifier extends ASTNode {
        name: 'err' | 'bool' | 'u8' | 'i8' | 'u16' | 'i16' | 'u32' | 'i32' | 'f32' | 'str'
        arr_length?: number
    }
    export class Identifier extends ASTNode {
        name: string
        type: TypeIdentifier
        children = () => [this.type]
    }

    // Values
    export class Node extends ASTNode {
        device: string
        port: string
        index?: number | string
    }

    export class Value extends ASTNode {
        value_type: 'Boolean'|'Hexcode'|'Bitmask'|'Float'|'UnsignedInteger'|'Integer'|'String'
        value: any
    }
}

export const quimblos_semantics = new SemanticsBuilder()
    .node('grammar',
        qbscript.Script, $ => ({
            comments: $.all('comment').optional,
            macros: $.all('macro').optional,
            declarations: $.all('declaration').optional,
            statements: $.all('statement').optional,
        })
    )
    .node('comment',
        qbscript.Comment, $ => ({})
    )

    // Macros
    
    .node('macro_use',
        qbscript.UseDeviceMacro, $ => ({
            device: $.first_text('identifier_device')
        })
    )

    // Declarations
    
    .node('var_declaration',
        qbscript.VariableDeclaration, $ => ({
            identifier: $.first('typed_identifier'),
            value: $.first('value').optional
        })
    )
    .node('ptr_declaration',
        qbscript.PointerDeclaration, $ => ({
            identifier: $.nth('identifier', 0),
            ref: $.first('ref_no_idx'),
            value: $.first('integer').optional
        })
    )

    // Statements

    .node('statement_assign',
        qbscript.AssignStatement, $ => ({
            target: $.first('ref'),
            source: $.first('expression_math'),
        })
    )
    .node('statement_if',
        qbscript.IfStatement, $ => ({
            expression: $.first('expression_bool')
        })
    )
    .node('statement_else',
        qbscript.ElseStatement, $ => ({})
    )
    .node('statement_else_if',
        qbscript.ElseIfStatement, $ => ({
            statement: $.first('statement_if')
        })
    )
    .node('statement_while',
        qbscript.WhileStatement, $ => ({
            expression: $.first('expression_bool')
        })
    )
    .node('statement_hold',
        qbscript.HoldStatement, $ => ({
            device: $.first_text('identifier_device')
        })
    )
    .node('statement_release',
        qbscript.HoldStatement, $ => ({
            device: $.first_text('identifier_device')
        })
    )
    .node('statement_log',
        qbscript.LogStatement, $ => ({
            value: $.first('value'),
        })
    )
    .node('statement_sleep',
        qbscript.SleepStatement, $ => ({
            time: $.first_text('unsigned_integer', v => parseInt(v))
        })
    )
    .node('statement_return',
        qbscript.ReturnStatement, $ => ({
            value: $.first('value').optional,
        })
    )
    .node('statement_reset',
        qbscript.ResetStatement, $ => ({})
    )
    .node('statement_reboot',
        qbscript.ResetStatement, $ => ({})
    )

    // Expressions

    .node('expression_bool',
        qbscript.BoolExpression, $ => ({
            terms: $.all('bool_term'),
            mods: $.all_text('bool_mod').optional
        })
    )
    .node('bool_term',
        qbscript.BoolExpressionTerm, $ => ({
            value: $.any([
                $.first('expression_bool'),
                $.first('bool_operation')
            ])
        })
    )
    .node('bool_operation',
        qbscript.BoolOperation, $ => ({
            left: $.nth('value', 0),
            right: $.nth('value', 1),
            op: $.first_text('op_compare')
        })
    )

    .node('expression_math',
        qbscript.MathExpression, $ => ({
            terms: $.all('math_term'),
            ops: $.all_text('op').optional
        })
    )
    .node('math_term',
        qbscript.MathExpressionTerm, $ => ({
            value: $.any([
                $.first('expression_math'),
                $.first('value')
            ])
        })
    )

    // References

    .node('ref_script',
        qbscript.Reference, $ => ({
            device: $.empty(),
            node: $.first_text('identifier'),
            index: $.empty(),
            ref: $.empty()
        })
    )
    .node('ref_script_idx',
        qbscript.Reference, $ => ({
            device: $.empty(),
            node: $.first_text('identifier'),
            index: $.any([
                $.first_text('integer', v => parseInt(v)),
                $.first('ref')
            ]).optional,
            ref: $.empty()
        })
    )
    .node('ref_device',
        qbscript.Reference, $ => ({
            device: $.first_text('identifier_device'),
            node: $.first_text('identifier'),
            index: $.empty(),
            ref: $.empty()
        })
    )
    .node('ref_device_idx',
        qbscript.Reference, $ => ({
            device: $.first_text('identifier_device'),
            node: $.first_text('identifier'),
            index: $.any([
                $.first_text('integer', v => parseInt(v)),
                $.first('ref')
            ]).optional,
            ref: $.empty()
        })
    )

    // Identifiers

    .node('typed_identifier',
        qbscript.Identifier, $ => ({
            name: $.first_text('identifier'),
            type: $.node(qbscript.TypeIdentifier, $ => ({
                name: $.first_text('identifier_type'),
                arr_length: $.first_text('unsigned_integer', v => parseInt(v)).optional
            }))
        })
    )
    .node('identifier',
        qbscript.Identifier, $ => ({
            name: $.text(),
            type: $.empty()
        })
    )

    // Values

    .node('boolean',
        qbscript.Value, $ => ({
            value_type: $.value('Boolean'),
            value: $.text(v => v === 'true')
        })
    )
    .node('hexcode',
        qbscript.Value, $ => ({
            value_type: $.value('Hexcode'),
            value: $.text()
        })
    )
    .node('bitmask',
        qbscript.Value, $ => ({
            value_type: $.value('Bitmask'),
            value: $.text()
        })
    )
    .node('float',
        qbscript.Value, $ => ({
            value_type: $.value('Float'),
            value: $.text(v => parseFloat(v))
        })
    )
    .node('integer',
        qbscript.Value, $ => ({
            value_type: $.value('Integer'),
            value: $.text(v => parseInt(v))
        })
    )
    .node('unsigned_integer',
        qbscript.Value, $ => ({
            value_type: $.value('UnsignedInteger'),
            value: $.text(v => parseInt(v))
        })
    )
    .node('string',
        qbscript.Value, $ => ({
            value_type: $.value('String'),
            value: $.text(v => v.slice(1,-1))
        })
    )

    .build();
