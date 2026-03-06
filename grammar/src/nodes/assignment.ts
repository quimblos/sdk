import { ASTNode } from "../node";
import { CSTNode } from "../cst";
import { QbError } from "../error";

export class Assignment extends ASTNode<'Assignment'> {
    public device?: string
    public port!: string
    public op!: '=' | '+=' | '-=' | '*=' | '/=' | '%=';
    public value!: string;
    constructor(
        parent: ASTNode<'Code'>,
        cst: CSTNode
    ) {
        super(parent, 'Assignment', cst);
    
        if (cst.type !== 'assignment') {
            this.errors.push(QbError.Syntax.WrongNode(cst, this, 'assignment'));
            return;
        }

        const variableNode = cst.children.find(c => c.type === 'kw_type')!;
        const variable = variableNode.text.trim().split('.');

        if (variable.length === 2) {
            this.device = variable[0];
            this.port = variable[1];
        }
        else {
            this.port = variable[0];
        }

        const opArtNode = cst.children.find(c => c.type === 'op_art');
        this.op = ((opArtNode?.text.trim() ?? '=') + '') as `${(''|'+'|'-'|'*'|'/'|'%')}=`;
    }
    public children() { return [] }
}