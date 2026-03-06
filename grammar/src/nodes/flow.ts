import { ASTNode } from "../node";
import { CSTNode } from "../cst";
import { QbError } from "../error";

export class IfStatement extends ASTNode<'IfStatement'> {
    public device?: string
    public port!: string
    public op!: '==' | '!=' | '>' | '<' | '>=' | '<=';
    constructor(
        parent: ASTNode<'Code'>,
        cst: CSTNode
    ) {
        super(parent, 'IfStatement', cst);
    
        if (cst.type !== 'if_statement') {
            this.errors.push(QbError.Syntax.WrongNode(cst, this, 'if_statement'));
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
        this.op = ((opArtNode?.text.trim() ?? '=') + '=') as `${('='|'+'|'-'|'*'|'/'|'%')}=`;
    }
    public children() { return [] }
}