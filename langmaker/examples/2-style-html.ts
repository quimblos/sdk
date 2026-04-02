import { make_parser } from "..";
import { to_html } from "../src/format/html";
import { HTMLStyle } from "../src/format/html";
import { ASTNode } from "../src/semantics/ast";
import { SemanticsBuilder } from "../src/semantics/semantics";

// Syntax

const syntax = `
grammar ::= identifier ws? '=' ws? numeric
identifier ::= [a-z]+
numeric ::= [0-9]+
ws ::= ' '+
`;

// Semantics

namespace my_lang {
    export class Script extends ASTNode {
        prop!: string
        value!: Numeric
        children = () => [this.value]
    }
    export class Numeric extends ASTNode {
        value!: number
    }
}

const semantics = new SemanticsBuilder()
    .node('grammar', my_lang.Script, $ => ({
        prop: $.first_text('identifier'),
        value: $.first('numeric')
    }))
    .node('numeric', my_lang.Numeric, $ => ({
        value: $.text(v => parseInt(v))
    }))
    .build();

// Linter

const style: HTMLStyle = {
    suffix: 'extra_text',
    syntax: {
        'identifier': 'red',
        'numeric': 'green'
    },
    css: {
        'red': { color: '#f00' },
        'green': { color: '#0f0' },
    }
};

// Parser

const parser = make_parser(syntax, semantics)

const code = 'abc = 123';

const ast = parser(code);
const html = to_html(code, ast, style);

console.log(html);