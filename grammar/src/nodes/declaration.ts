import { ASTNode } from "../node";
import { CSTNode } from "../cst";
import { QbError } from "../error";
import { Type } from "../type";

export class Declaration extends ASTNode<'Declaration'> {
    public type!: Type
    public name!: string
    constructor(
        parent: ASTNode<'Code'>,
        cst: CSTNode
    ) {
        super(parent, 'Declaration', cst);
    
        if (cst.type !== 'declaration') {
            this.errors.push(QbError.Syntax.WrongNode(cst, this, 'declaration'));
            return;
        }

        const typeNode = cst.children.find(c => c.type === 'kw_type')!;
        this.type = typeNode.text.trim() as Type;

        const nameNode = cst.children.find(c => c.type === 'word')!;
        this.name = nameNode.text.trim();
    }
    public children() { return [] }
}