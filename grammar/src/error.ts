import { CSTNode } from './cst';
import { ASTNode } from './node';

/*
    - Wrong: A different CST node was expected,
        this is considered "wrong" syntax, since
        it can be parsed (it's "valid") but expresses
        the wrong thing from a semantics point.

    - NotFound: A node of the AST was not found
*/

export namespace QbError {

    export class Base {
        constructor(
            public name: string,
            public reason: string,
            public details: string[],
            public node: ASTNode<any>,
            public data: Record<string, any> = {}
        ) {}
    }


    export namespace Syntax {

        export function Error(node: ASTNode<any>, error: string) {
            return new QbError.Base(
                'SyntaxError',
                'Syntax error',
                [error],
                {
                    ...node,
                    start: node.end,
                    end: Number.MAX_VALUE,
                } as ASTNode<any>
            );
        }
        
        export function WrongNode(cst: CSTNode, ast: ASTNode<any>, expected: string) {
            return new QbError.Base(
                'Syntax.Wrong',
                `Wrong '${cst.type}' node`,
                [`Expected \'${expected}\' node`],
                ast
            );
        }
    }


    export namespace Semantics {

    }

    export namespace Type {


    }


}