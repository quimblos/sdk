import { make_parser } from "..";
import { ASTNode } from "../src/semantics/ast";
import { SemanticsBuilder } from "../src/semantics/semantics";

// Syntax

const syntax = `
grammar ::= hello ' ' world
hello ::= 'hello'
world ::= 'world'
`;

// Semantics

export namespace my_lang {
    export class Script extends ASTNode {
        hello!: HelloStatement
        world!: WorldStatement
        children = () => [this.hello, this.world]
    }
    export class HelloStatement extends ASTNode {}
    export class WorldStatement extends ASTNode {}
}

export const semantics = new SemanticsBuilder()
    .node('grammar', my_lang.Script, $ => ({
        hello: $.first('hello'),
        world: $.first('world')
    }))
    .node('hello', my_lang.HelloStatement, $ => ({}))
    .node('world', my_lang.WorldStatement, $ => ({}))
    .build();

// Parser

const parser = make_parser(syntax, semantics)

const code = 'hello world';

const ast = parser(code);
console.log(ast);