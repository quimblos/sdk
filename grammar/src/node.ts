import { CSTNode } from "./cst"
import { QbError } from "./error"

export type ASTNodeKind =
      'Code'
      | 'Declaration'
      | 'SleepStatement'
      | 'LogStatement'
      | 'StopStatement'
      | 'ErrorStatement'
      | 'Assignment'
      | 'IfStatement'
    

export abstract class ASTNode<Kind extends ASTNodeKind> {
    public parent: ASTNode<ASTNodeKind>
    public kind: Kind
    public start: number
    public end: number
    public text: string
    public errors: QbError.Base[]

    constructor(
        parent: ASTNode<ASTNodeKind>,
        kind: Kind,
        cst: CSTNode
    ) {
        this.parent = parent;
        this.kind = kind;
        this.start = cst.start;
        this.end = cst.end;
        this.text = cst.text;
        this.errors = cst.errors.map(err => QbError.Syntax.Error(this, err.message));
    }

    abstract children(): (ASTNode<ASTNodeKind> | undefined)[]

    public traverse(fn: (node: ASTNode<ASTNodeKind>) => boolean|void) {
        const next: ASTNode<ASTNodeKind>[] = [this];
        for (const node of next) {
            if (fn(node) === true) return;
            for (const child of node.children()) {
                if (child !== undefined) {
                    next.push(child);
                }
            }
        }
    }

    public allErrors() {
        const errors: QbError.Base[] = [];
        this.traverse(node => {
            errors.push(...node.errors);
        })
        return errors;
    }

    public find(predicate: (node: ASTNode<ASTNodeKind>) => boolean) {
        const children = this.children();
        for (const child of children) {
            if (!child) continue;
            if (predicate(child)) return child;
            children.push(...child.children());
        }
    }

    public findUp(predicate: (node: ASTNode<ASTNodeKind>) => boolean) {
        let parent = this.parent;
        while (parent !== undefined) {
            if (predicate(parent)) return parent;
            parent = parent.parent;
        }
    }
}