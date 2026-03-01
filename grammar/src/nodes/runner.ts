import { ASTNode } from "../node";
import { CSTNode } from "../cst";
import { QbError } from "../error";
import { Type } from "../type";

export class SleepStatement extends ASTNode<'SleepStatement'> {
    public ms!: number
    constructor(
        parent: ASTNode<'Code'>,
        cst: CSTNode
    ) {
        super(parent, 'SleepStatement', cst);
    
        if (cst.type !== 'sleep_statement') {
            this.errors.push(QbError.Syntax.WrongNode(cst, this, 'sleep_statement'));
            return;
        }

        const valueNode = cst.children.find(c => c.type === 'uint')!;
        this.ms = parseInt(valueNode.text.trim());

    }
    public children() { return [] }
}

export class LogStatement extends ASTNode<'LogStatement'> {
    public log_code?: number
    public output!: string
    constructor(
        parent: ASTNode<'Code'>,
        cst: CSTNode
    ) {
        super(parent, 'LogStatement', cst);
    
        if (cst.type !== 'log_statement') {
            this.errors.push(QbError.Syntax.WrongNode(cst, this, 'log_statement'));
            return;
        }

        const logCodeNode = cst.children.find(c => c.type === 'uint')!;
        if (logCodeNode) {
            this.log_code = parseInt(logCodeNode.text.trim());
        }

        const outputNode = cst.children.find(c => c.type === 'string')!;
        this.output = outputNode.text.trim();

    }
    public children() { return [] }
}

export class StopStatement extends ASTNode<'StopStatement'> {
    public log_code?: number
    public output!: string
    constructor(
        parent: ASTNode<'Code'>,
        cst: CSTNode
    ) {
        super(parent, 'StopStatement', cst);
    
        if (cst.type !== 'stop_statement') {
            this.errors.push(QbError.Syntax.WrongNode(cst, this, 'stop_statement'));
            return;
        }

        const logCodeNode = cst.children.find(c => c.type === 'uint')!;
        if (logCodeNode) {
            this.log_code = parseInt(logCodeNode.text.trim());
        }

        const outputNode = cst.children.find(c => c.type === 'string')!;
        this.output = outputNode.text.trim();

    }
    public children() { return [] }
}

export class ErrorStatement extends ASTNode<'ErrorStatement'> {
    public log_code?: number
    public output!: string
    constructor(
        parent: ASTNode<'Code'>,
        cst: CSTNode
    ) {
        super(parent, 'ErrorStatement', cst);
    
        if (cst.type !== 'error_statement') {
            this.errors.push(QbError.Syntax.WrongNode(cst, this, 'error_statement'));
            return;
        }

        const logCodeNode = cst.children.find(c => c.type === 'uint')!;
        if (logCodeNode) {
            this.log_code = parseInt(logCodeNode.text.trim());
        }

        const outputNode = cst.children.find(c => c.type === 'string')!;
        this.output = outputNode.text.trim();

    }
    public children() { return [] }
}