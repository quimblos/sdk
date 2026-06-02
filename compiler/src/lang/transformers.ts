import { Transformer, CSTNode } from "@quimblos/langmaker";
import { quimblos } from "./semantics";

type ParentStatement =
    quimblos.IfStatement
    | quimblos.ElseStatement
    | quimblos.ElseIfStatement
    | quimblos.WhileStatement

export const quimblos_identation_transformer = new Transformer()
    .ast_rule((ast, { error }) => {
        const script = ast.root as quimblos.Script;
        if (script.statements.length < 2) return;

        const blanks = script.cst.children!.filter(
            group => group
                .children!.find(c => c.kind === '_group')
                ?.children!.some(c => c.kind === 'statement')
        )
            .map(group => group
                .children!.find(c => c.kind === 'blank')
            );

        const _tab = (blank?: CSTNode) => {
            if (!blank) return 0;
            const idx = blank.text.lastIndexOf('\n');
            if (idx < 0) return blank.text.length;
            return blank.text.length - idx - 1;
        }

        let tabs = [_tab(blanks[0])];
        
        for (let i = 0; i < script.statements.length; i++) {
            const statement = script.statements[i]!;
            const tab = _tab(blanks[i]);

            const next_statement = script.statements[i+1];
            const next_tab = _tab(blanks[i+1]);

            if (tabs.length > 1) {
                if (tab > tabs[0]!) {
                    error(statement, blanks[i] ?? statement.cst, `Identation error`);
                    continue
                }
                const tab_idx = tabs.findIndex(t => t == tab!)
                if (tab_idx < 0) {
                    error(statement, blanks[i] ?? statement.cst, `Identation error`);
                    continue
                }
                tabs = tabs.slice(tab_idx)
            }
            else {
                if (tab != tabs[0]) {
                    error(statement, blanks[i] ?? statement.cst, `Identation error`);
                    continue
                }
            }
            
            statement.cst.tab = tabs.length-1;

            if (blanks[i] && tabs.length > 1) {
                let last_nl = blanks[i]!.text.lastIndexOf('\n');
                if (last_nl < 0) last_nl = -1;
                const n = blanks[i]!.text.length;
                let text = '';
                for (let j = 0; j < n; j++) {
                    const tab_idx = j - last_nl - 1
                    const idx = tabs.indexOf(tab_idx);
                    if (tab_idx >= 0 && idx >= 0) {
                        if (next_tab < tab && 
                            (
                                !(next_statement instanceof quimblos.ElseStatement)
                                && !(next_statement instanceof quimblos.ElseIfStatement)
                            )) {
                            text += '└'
                        }
                        else {
                            if (idx > 1) {
                                text += '│'
                            }
                            else {
                                text += '├'
                            }
                        }
                    }
                    else {
                        text += blanks[i]!.text[j];
                    }

                }
                const c = '<a\tclass="iden">' + text + '</a>'
                blanks[i]!.text = c;
            }

            if (
                statement instanceof quimblos.IfStatement
                || statement instanceof quimblos.ElseStatement
                || statement instanceof quimblos.ElseIfStatement
                || statement instanceof quimblos.WhileStatement
            ) {                
                if (next_statement) {
                    if (next_tab <= tabs[0]!) {
                        error(next_statement, blanks[i+1] ?? next_statement.cst, `Identation error`);
                        continue;
                    }
                    tabs.unshift(next_tab!);
                }
            }
        }
    })

export const quimblos_block_transformer = new Transformer()
    .ast_rule(ast => {
        if (ast.errors.length) return;
        const script = ast.root as quimblos.Script;

        const main: quimblos.Block = new quimblos.Block();
        let stack = [main];

        for (const statement of script.statements) {
            const block0 = stack[0]!;
            if (statement.cst.tab != null) {
                if (statement.cst.tab > block0.depth) {
                    const parent = block0.statements.at(-1) as ParentStatement;
                    
                    const block1 = new quimblos.Block();
                    block1.depth = block0.depth+1;
                    parent.block = block1;
                    
                    stack.unshift(block1);
                }
                else if (statement.cst.tab < block0.depth) {
                    if (block0.depth === 0) throw new Error(`Identation error passed through linter, weird situation.`);
                    while (stack[0]!.depth > statement.cst.tab) {
                        stack.shift()!;
                    }
                }
            }
            stack[0]!.statements.push(statement);
        }

        script.blocks = [main];
    })