import { ASTNode, SemanticsBuilder } from "@quimblos/langmaker"
import { TypeDef } from "./types"

export namespace quimblos {
    export type LiteralType = quimblos.Literal['literal_type']
    export type Value = Expression | Literal | Reference

    export class Script extends ASTNode {
        macros!: Macro[]
        statements!: Statement[]
        blocks!: Block[]
        children = () => [...this.macros, ...this.blocks]
    }

    export class Block extends ASTNode {
        depth: number = 0
        statements: Statement[] = []
        children = () => [...this.statements]
    }

    // Macros

    export type Macro = UseDriverMacro | UseTopicMacro;
    export class UseDriverMacro extends ASTNode {
        device!: string
    }
    export class UseTopicMacro extends ASTNode {
        topic!: string
    }

    // Declarations
    
    export class TypeDeclaration extends ASTNode {
        identifier!: Identifier
        type!: Identifier | Type
    }

    export class VariableDeclaration extends ASTNode {
        identifier!: Identifier
        value?: Expression
        children = () => [this.identifier, this.value]
    }

    // Statements

    export type Statement = HoldStatement | ReleaseStatement | LogStatement | SleepStatement | ReturnStatement | RebootStatement
    export class AssignStatement extends ASTNode {
        target!: Reference
        source!: Expression
        children = () => [this.target, this.source]
    }
    export class IfStatement extends ASTNode {
        expression!: Expression
        block!: Block
        children = () => [this.expression, this.block]
    }
    export class ElseStatement extends ASTNode {
        block!: Block
        children = () => [this.block]
    }
    export class ElseIfStatement extends ASTNode {
        statement!: IfStatement
        block!: Block
        children = () => [this.statement, this.block]
    }
    export class WhileStatement extends ASTNode {
        expression!: Expression
        block!: Block
        children = () => [this.expression, this.block]
    }
    export class HoldStatement extends ASTNode {
        device!: string
    }
    export class ReleaseStatement extends ASTNode {
        device!: string
    }
    export class LogStatement extends ASTNode {
        value!: Expression
        children = () => [this.value]
    }
    export class SleepStatement extends ASTNode {
        time!: Expression
    }
    export class ReturnStatement extends ASTNode {
        value!: Expression
        children = () => [this.value]
    }
    export class RebootStatement extends ASTNode {}

    // Expressions
    export type BoolOp = 'not'|'and'|'or'|'xor'|'=='|'!='|'>'|'<'|'>='|'<='
    export type MathOp = '+'|'-'|'*'|'/'|'%'|'^'
    export class Expression extends ASTNode {
        terms!: ExpressionTerm[]
        ops!: (BoolOp | MathOp)[]
        children = () => [...this.terms]
    }
    export class ExpressionTerm extends ASTNode {
        value!: Value
        children = () => [this.value]
    }

    // References
    export class Reference extends ASTNode {
        device?: string
        node!: string
        index?: Expression
        ref!: Identifier
        children = () => [this.index, this.ref]
    }
    
    // Identifiers

    export class TypeIdentifier extends ASTNode {
        name!: 'err' | 'null' | 'void' | 'bool' | 'u8' | 'i8' | 'u16' | 'i16' | 'u32' | 'i32' | 'f32' | 'str'
        arr_length?: number
    }
    export class Identifier extends ASTNode {
        name!: string
        type?: TypeIdentifier
        children = () => [this.type]
    }

    // Types

    export type Type = MapType | StructType
    export class MapType extends ASTNode {
        kind!: 'vec' | 'map' | 'event'
        type!: Identifier|Type
    }
    export class StructType extends ASTNode {
        fields!: StructFieldType[]
    }
    export class StructFieldType extends ASTNode {
        key!: string
        type!: Identifier|Type
    }

    // Values
    export class Node extends ASTNode {
        device!: string
        port!: string
        index?: number | string
    }

    export class Literal extends ASTNode {
        literal_type!: 'Boolean'|'Hexcode'|'Bitmask'|'Float'|'UnsignedInteger'|'Integer'|'String'
        value: any
    }
}

export const quimblos_semantics = new SemanticsBuilder()
    .node('grammar',
        quimblos.Script, $ => ({
            macros: $.all('macro').optional,
            statements: $.all('statement').optional,
            blocks: $.value([] as any)
        })
    )

    // Macros
    
    .node('macro_use_driver',
        quimblos.UseDriverMacro, $ => ({
            device: $.first_text('identifier')
        })
    )
    .node('macro_use_topic',
        quimblos.UseTopicMacro, $ => ({
            topic: $.first_text('identifier')
        })
    )

    // Declarations
    
    .node('declaration_type',
        quimblos.TypeDeclaration, $ => ({
            identifier: $.first('identifier'),
            type: $.first('type')
        })
    )
    
    .node('declaration_var',
        quimblos.VariableDeclaration, $ => ({
            identifier: $.any([
                $.first('typed_identifier'),
                $.first('identifier')
            ]),
            value: $.first('expression').optional
        })
    )

    // Statements

    .node('statement_assign',
        quimblos.AssignStatement, $ => ({
            target: $.first('reference'),
            source: $.first('expression'),
        })
    )
    .node('statement_if',
        quimblos.IfStatement, $ => ({
            expression: $.first('expression'),
            block: $.empty()
        })
    )
    .node('statement_else',
        quimblos.ElseStatement, $ => ({
            block: $.empty()
        })
    )
    .node('statement_else_if',
        quimblos.ElseIfStatement, $ => ({
            statement: $.first('statement_if'),
            block: $.empty()
        })
    )
    .node('statement_while',
        quimblos.WhileStatement, $ => ({
            expression: $.first('expression'),
            block: $.empty()
        })
    )
    .node('statement_hold',
        quimblos.HoldStatement, $ => ({
            device: $.first_text('identifier_device')
        })
    )
    .node('statement_release',
        quimblos.HoldStatement, $ => ({
            device: $.first_text('identifier_device')
        })
    )
    .node('statement_log',
        quimblos.LogStatement, $ => ({
            value: $.first('expression'),
        })
    )
    .node('statement_sleep',
        quimblos.SleepStatement, $ => ({
            time: $.first('expression')
        })
    )
    .node('statement_return',
        quimblos.ReturnStatement, $ => ({
            value: $.first('expression').optional,
        })
    )
    .node('statement_reboot',
        quimblos.RebootStatement, $ => ({})
    )

    // Expressions

    .node('expression',
        quimblos.Expression, $ => ({
            terms: $.all('term'),
            ops: $.all_text('op').optional
        })
    )
    .node('term',
        quimblos.ExpressionTerm, $ => ({
            value: $.any([
                $.first('expression'),
                $.first('value')
            ])
        })
    )

    // References

    .node('ref_script',
        quimblos.Reference, $ => ({
            device: $.empty(),
            node: $.first_text('identifier'),
            index: $.empty(),
            ref: $.empty()
        })
    )
    .node('ref_script_idx',
        quimblos.Reference, $ => ({
            device: $.empty(),
            node: $.first_text('identifier'),
            index: $.first('expression'),
            ref: $.empty()
        })
    )
    .node('ref_device',
        quimblos.Reference, $ => ({
            device: $.first_text('identifier_device'),
            node: $.first_text('identifier'),
            index: $.empty(),
            ref: $.empty()
        })
    )
    .node('ref_device_idx',
        quimblos.Reference, $ => ({
            device: $.first_text('identifier_device'),
            node: $.first_text('identifier'),
            index: $.first('expression'),
            ref: $.empty()
        })
    )

    // Identifiers

    .node('typed_identifier',
        quimblos.Identifier, $ => ({
            name: $.first_text('identifier'),
            type: $.node(quimblos.TypeIdentifier, $ => ({
                name: $.first_text('identifier_type'),
                arr_length: $.first_text('unsigned_integer', v => parseInt(v)).optional
            }))
        })
    )
    .node('identifier',
        quimblos.Identifier, $ => ({
            name: $.text(),
            type: $.empty()
        })
    )

    // Types

    .node('type_vec',
        quimblos.MapType, $ => ({
            kind: $.value('vec'),
            type: $.first('type')
        })
    )
    .node('type_map',
        quimblos.MapType, $ => ({
            kind: $.value('map'),
            type: $.first('type')
        })
    )
    .node('type_event',
        quimblos.MapType, $ => ({
            kind: $.value('event'),
            type: $.first('type')
        })
    )
    .node('type_struct',
        quimblos.StructType, $ => ({
            kind: $.value('struct'),
            fields: $.all('type_struct_field')
        })
    )
    .node('type_struct_field',
        quimblos.StructFieldType, $ => ({
            key: $.first_text('identifier'),
            type: $.first('type')
        })
    )

    // Values

    .node('boolean',
        quimblos.Literal, $ => ({
            literal_type: $.value('Boolean'),
            value: $.text(v => v === 'true')
        })
    )
    .node('hexcode',
        quimblos.Literal, $ => ({
            literal_type: $.value('Hexcode'),
            value: $.text()
        })
    )
    .node('bitmask',
        quimblos.Literal, $ => ({
            literal_type: $.value('Bitmask'),
            value: $.text()
        })
    )
    .node('float',
        quimblos.Literal, $ => ({
            literal_type: $.value('Float'),
            value: $.text(v => parseFloat(v))
        })
    )
    .node('integer',
        quimblos.Literal, $ => ({
            literal_type: $.value('Integer'),
            value: $.text(v => parseInt(v))
        })
    )
    .node('unsigned_integer',
        quimblos.Literal, $ => ({
            literal_type: $.value('UnsignedInteger'),
            value: $.text(v => parseInt(v))
        })
    )
    .node('string',
        quimblos.Literal, $ => ({
            literal_type: $.value('String'),
            value: $.text(v => v.slice(1,-1))
        })
    )

    .build();
