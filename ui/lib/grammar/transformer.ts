import { Transformer } from "../lang-maker/transformer";
import { qbscript } from "./semantics";

type ParentStatement =
    qbscript.IfStatement
    | qbscript.ElseStatement
    | qbscript.ElseIfStatement
    | qbscript.WhileStatement


export const quimblos_transformer = new Transformer()
    
    // Group statements into blocks
    .rule(ast => {
        const script = ast.root as qbscript.Script;

        const main: qbscript.Block = new qbscript.Block();
        let stack = [main];

        for (const statement of script.statements) {
            const block0 = stack[0];
            if (statement.cst.tab > block0.depth) {
                const parent = block0.statements.at(-1) as ParentStatement;
                
                const block1 = new qbscript.Block();
                block1.depth = block0.depth+1;
                parent.block = block1;
                
                stack.unshift(block1);
            }
            else if (statement.cst.tab < block0.depth) {
                if (block0.depth === 0) throw new Error(`Identation error passed through linter, weird situation.`);
                while (stack[0].depth > statement.cst.tab) {
                    stack.shift();
                }
            }
            stack[0].statements.push(statement);
        }

        script.blocks = [main];
    })
