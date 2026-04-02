import { ASTNode } from "../lang-maker/ast";
import { SemanticsBuilder } from "../lang-maker/semantics";

export namespace qbscript {
    export type Type = NonNullable<qbscript.TypeIdentifier['name']>
    export type LiteralType = qbscript.Literal['literal_type']
    export type Value = Expression | Literal | Reference

    export class Script extends ASTNode {
        macros: Macro[]
        statements: Statement[]
        blocks: Block[]
        children = () => [...this.macros, ...this.statements, ...this.blocks]
    }

    export class Block extends ASTNode {
        depth: number = 0
        statements: Statement[] = []
        children = () => [...this.statements]
    }

    // Macros
    export type Macro = UseDeviceMacro;
    export class UseDeviceMacro extends ASTNode {
        device: string
    }

    export class VariableStatement extends ASTNode {
        identifier: Identifier
        value: Expression
        children = () => [this.identifier, this.value]
    }
    export class PointerStatement extends ASTNode {
        identifier: Identifier
        ref: Reference
        value?: Expression
        children = () => [this.identifier, this.ref, this.value]
    }

    // Statements
    export type Statement = HoldStatement | ReleaseStatement | LogStatement | SleepStatement | ReturnStatement | ResetStatement | RebootStatement
    export class AssignStatement extends ASTNode {
        target: Reference
        source: Expression
        children = () => [this.target, this.source]
    }
    export class IfStatement extends ASTNode {
        expression: Expression
        block: Block
        children = () => [this.expression, this.block]
    }
    export class ElseStatement extends ASTNode {
        block: Block
        children = () => [this.block]
    }
    export class ElseIfStatement extends ASTNode {
        statement: IfStatement
        block: Block
        children = () => [this.statement, this.block]
    }
    export class WhileStatement extends ASTNode {
        expression: Expression
        block: Block
        children = () => [this.expression, this.block]
    }
    export class HoldStatement extends ASTNode {
        device: string
    }
    export class ReleaseStatement extends ASTNode {
        device: string
    }
    export class LogStatement extends ASTNode {
        value: Expression
        children = () => [this.value]
    }
    export class SleepStatement extends ASTNode {
        time: number
    }
    export class ReturnStatement extends ASTNode {
        value: Expression
        children = () => [this.value]
    }
    export class ResetStatement extends ASTNode {}
    export class RebootStatement extends ASTNode {}

    // Expressions
    export type BoolOp = 'and'|'or'|'=='|'!='|'>'|'<'|'>='|'<='
    export type MathOp = '+'|'-'|'*'|'/'|'%'|'^'
    export class Expression extends ASTNode {
        terms: ExpressionTerm[]
        ops: (BoolOp | MathOp)[]
        children = () => [...this.terms]
    }
    export class ExpressionTerm extends ASTNode {
        value: Value
        children = () => [this.value]
    }

    // References
    export class Reference extends ASTNode {
        device?: string
        node: string
        index?: Expression
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

    export class Literal extends ASTNode {
        literal_type: 'Boolean'|'Hexcode'|'Bitmask'|'Float'|'UnsignedInteger'|'Integer'|'String'
        value: any
    }
}

export const quimblos_semantics = new SemanticsBuilder()
    .node('grammar',
        qbscript.Script, $ => ({
            macros: $.all('macro').optional,
            statements: $.all('statement').optional,
            blocks: $.value([] as any)
        })
    )

    // Macros
    
    .node('macro_use',
        qbscript.UseDeviceMacro, $ => ({
            device: $.first_text('identifier_device')
        })
    )

    // Statements
    
    .node('statement_var',
        qbscript.VariableStatement, $ => ({
            identifier: $.first('typed_identifier'),
            value: $.first('expression').optional
        })
    )
    .node('statement_ptr',
        qbscript.PointerStatement, $ => ({
            identifier: $.nth('identifier', 0),
            ref: $.first('ref_no_idx'),
            value: $.first('expression').optional
        })
    )

    .node('statement_assign',
        qbscript.AssignStatement, $ => ({
            target: $.first('reference'),
            source: $.first('expression'),
        })
    )
    .node('statement_if',
        qbscript.IfStatement, $ => ({
            expression: $.first('expression_bool'),
            block: $.empty()
        })
    )
    .node('statement_else',
        qbscript.ElseStatement, $ => ({
            block: $.empty()
        })
    )
    .node('statement_else_if',
        qbscript.ElseIfStatement, $ => ({
            statement: $.first('statement_if'),
            block: $.empty()
        })
    )
    .node('statement_while',
        qbscript.WhileStatement, $ => ({
            expression: $.first('expression_bool'),
            block: $.empty()
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
            value: $.first('expression'),
        })
    )
    .node('statement_sleep',
        qbscript.SleepStatement, $ => ({
            time: $.first_text('unsigned_integer', v => parseInt(v))
        })
    )
    .node('statement_return',
        qbscript.ReturnStatement, $ => ({
            value: $.first('expression').optional,
        })
    )
    .node('statement_reset',
        qbscript.ResetStatement, $ => ({})
    )
    .node('statement_reboot',
        qbscript.ResetStatement, $ => ({})
    )

    // Expressions

    .node('expression',
        qbscript.Expression, $ => ({
            terms: $.all('term'),
            ops: $.all_text('op').optional
        })
    )
    .node('term',
        qbscript.ExpressionTerm, $ => ({
            value: $.any([
                $.first('expression'),
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
            index: $.first('expression'),
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
            index: $.first('expression'),
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
        qbscript.Literal, $ => ({
            literal_type: $.value('Boolean'),
            value: $.text(v => v === 'true')
        })
    )
    .node('hexcode',
        qbscript.Literal, $ => ({
            literal_type: $.value('Hexcode'),
            value: $.text()
        })
    )
    .node('bitmask',
        qbscript.Literal, $ => ({
            literal_type: $.value('Bitmask'),
            value: $.text()
        })
    )
    .node('float',
        qbscript.Literal, $ => ({
            literal_type: $.value('Float'),
            value: $.text(v => parseFloat(v))
        })
    )
    .node('integer',
        qbscript.Literal, $ => ({
            literal_type: $.value('Integer'),
            value: $.text(v => parseInt(v))
        })
    )
    .node('unsigned_integer',
        qbscript.Literal, $ => ({
            literal_type: $.value('UnsignedInteger'),
            value: $.text(v => parseInt(v))
        })
    )
    .node('string',
        qbscript.Literal, $ => ({
            literal_type: $.value('String'),
            value: $.text(v => v.slice(1,-1))
        })
    )

    .build();
