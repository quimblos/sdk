import { make_parser } from "..";
import { ASTNode } from "../src/semantics/ast";
import { SemanticsBuilder } from "../src/semantics/semantics";
import { Linter } from "../src/static/linter";

// Syntax

const syntax = `
grammar ::= identifier ws? '=' ws? numeric
identifier ::= [a-z]+
numeric ::= [0-9]+
ws ::= ' '+
`;

// Semantics

export namespace my_lang {
    export class Script extends ASTNode {
        prop!: string
        value!: Numeric
        children = () => [this.value]
    }
    export class Numeric extends ASTNode {
        value!: number
    }
}

export const semantics = new SemanticsBuilder()
    .node('grammar', my_lang.Script, $ => ({
        prop: $.first_text('identifier'),
        value: $.first('numeric')
    }))
    .node('numeric', my_lang.Numeric, $ => ({
        value: $.text(v => parseInt(v))
    }))
    .build();

// Linter

export const linter = new Linter<my_lang.Script>()
    .rule(my_lang.Numeric, (node, { error }) => {
        if (node.value > 100) {
            error(node.cst, 'Value can\'t be greater than 100')
            return true;
        }
    })

// Parser

const parser = make_parser(syntax, semantics, linter)

const code_ok = 'abc = 99';

const ast_ok = parser(code_ok);
console.log('OK', ast_ok.errors);

const code_error = 'abc = 101';

const ast_error = parser(code_error);
console.log('ERROR', ast_error.errors);