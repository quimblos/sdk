import path from 'path';
import fs from 'fs';
import { Grammars } from 'ebnf';
import { AST } from './src/ast';
import { CSTNode } from './src/cst';
import { TypeChecker } from './src/checkers/type.checker';

export class Grammar {
    
    private parser: Grammars.W3C.Parser;

    constructor() {
        const grammarPath = path.join(__dirname, 'syntax', 'lila.ebnf');
        const grammar = fs.readFileSync(grammarPath).toString();
        this.parser = new Grammars.W3C.Parser(grammar);   
    }

    parse(text: string) {
        const cst = this.parser.getAST(text+'\n') as CSTNode;
        if (!cst) return;
        const ast = new AST(cst);
        return ast;
    }

    check(ast: AST) {
        TypeChecker.check(ast);
        return ast;
    }

}

export { AST } from './src/ast';
export * from './src/nodes/assignment'
export * from './src/nodes/context'
export * from './src/nodes/declaration'
export * from './src/nodes/expression'
export * from './src/nodes/identifier'
export * from './src/nodes/statement'
export * from './src/nodes/value'