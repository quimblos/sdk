import { AST, ASTNode } from "./ast"
import { CSTNode } from "./cst"

type StaticRule<
    T extends typeof ASTNode
> = (
    node: InstanceType<T>,
    $: {
        root: ASTNode,
        error: (cst: CSTNode, err: string) => void
    }
) => void

export class Linter {

    private rules: {
        node: { new (...args: any[]): ASTNode },
        rule: StaticRule<any>
    }[] = []

    public rule<
        T extends typeof ASTNode
    >(node: T, rule: StaticRule<T>) {
        this.rules.push({ node, rule })
        return this;
    }

    public lint(ast: AST) {
        ast.traverse(node => {
            const node_rules = this.rules.filter(rule => node instanceof rule.node);
            if (!node_rules.length) return;
            for (const rule of node_rules) {
                rule.rule(node, {
                    root: ast.root,
                    error: (cst: CSTNode, error: string) => {
                        ast.errors.push({ ast: node, cst, pos: '0', error });
                        cst.errors ??= []
                        cst.errors.push(error);
                    }
                })
            }
        })
    }

}