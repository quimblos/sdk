import { ASTNode } from "../lang-maker/ast";
import { SemanticsBuilder } from "../lang-maker/semantics";

export namespace qbscript {
    export type Type = NonNullable<qbscript.Identifier['type']>
    export type ValueType = qbscript.Value['value_type']

    export class Script extends ASTNode {
        comments: Comment[]
        macros: Macro[]
        declarations: Declaration[]
        children = () => [...this.comments, ...this.macros, ...this.declarations]
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
        value: Value
        children = () => [this.identifier, this.value]
    }
    export class PointerDeclaration extends ASTNode {
        identifier: Identifier
        port: PortIdentifier
        value: Value
        children = () => [this.identifier, this.port, this.value]
    }

    // Statements

    // Identifiers
    export class Identifier extends ASTNode {
        name: string
        type: 'err' | 'bool' | 'u8' | 'i8' | 'u16' | 'i16' | 'u32' | 'i32' | 'f32' | 'str' | 'arr' | 'unknown'
    }
    export class PortIdentifier extends ASTNode {
        device: string
        port: string
    }

    // Values
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
            declarations: $.all('declaration').optional
        })
    )
    .node('comment',
        qbscript.Comment, $ => ({})
    )

    // Macros
    
    .node('macro_use',
        qbscript.UseDeviceMacro, $ => ({
            device: $.first_text('identifier')
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
            port: $.first('port_identifier'),
            value: $.first('value').optional
        })
    )

    // Identifiers

    .node('port_identifier',
        qbscript.PortIdentifier, $ => ({
            device: $.nth_text('identifier', 0),
            port: $.nth_text('identifier', 1)
        })
    )
    .node('typed_identifier',
        qbscript.Identifier, $ => ({
            name: $.first_text('identifier'),
            type: $.first_text('type_identifier')
        })
    )
    .node('identifier',
        qbscript.Identifier, $ => ({
            name: $.text(),
            type: $.value('unknown')
        })
    )

    .node('typed_identifier',
        qbscript.Identifier, $ => ({
            name: $.first_text('identifier'),
            type: $.first_text('type_identifier')
        })
    )

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
