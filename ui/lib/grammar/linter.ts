import { Device } from "../kernel";
import { CST, CSTNode } from "../lang-maker/cst";
import { Linter } from "../lang-maker/linter";
import { qbscript } from "./semantics";
import { quimblos_types } from "./types";

export const quimblos_linter = new Linter<qbscript.Script>()
    
    // Identation
    .ast_rule((ast, { error }) => {
        const script = ast.root as qbscript.Script;
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
            const statement = script.statements[i];
            const tab = _tab(blanks[i]);

            const next_statement = script.statements[i+1];
            const next_tab = _tab(blanks[i+1]);

            if (tabs.length > 1) {
                if (tab > tabs[0]) {
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
            
            if (blanks[i] && tabs.length > 1) {
                let last_nl = blanks[i].text.lastIndexOf('\n');
                if (last_nl < 0) last_nl = -1;
                const n = blanks[i].text.length;
                let text = '';
                for (let j = 0; j < n; j++) {
                    const tab_idx = j - last_nl - 1
                    const idx = tabs.indexOf(tab_idx);
                    if (tab_idx >= 0 && idx >= 0) {
                        if (next_tab < tab && 
                            (
                                !(next_statement instanceof qbscript.ElseStatement)
                                && !(next_statement instanceof qbscript.ElseIfStatement)
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
                        text += blanks[i].text[j];
                    }

                }
                const c = '<a\tclass="iden">' + text + '</a>'
                blanks[i].text = c;
            }

            if (
                statement instanceof qbscript.IfStatement
                || statement instanceof qbscript.ElseStatement
                || statement instanceof qbscript.ElseIfStatement
                || statement instanceof qbscript.WhileStatement
            ) {                
                if (next_statement) {
                    if (next_tab <= tabs[0]) {
                        error(next_statement, blanks[i+1] ?? next_statement.cst, `Identation error`);
                        continue;
                    }
                    tabs.unshift(next_tab!);
                }
            }
        }
    })

    .rule(qbscript.UseDeviceMacro, (ast, { error }) => {
        try {
            check_engine_device(ast.device);
        }
        catch(e) {
            error(CST.first(ast.cst, 'identifier_device'), e);
        }
    })
    .rule(qbscript.VariableDeclaration, (ast, { root, error }) => {
        if (ast.value) {
            if (!ast.identifier) return;
            if (ast.identifier.type.arr_length) {
                error(CST.first(ast.cst, '_group'), 'Cannot assign to array');
                return
            }
            try {
                check_assign(ast.identifier.type, ast.value);
            }
            catch(e) {
                error(CST.first(ast.cst, 'value'), e);
            }
        }
    })
    .rule(qbscript.PointerDeclaration, (ast, { root, error }) => {
        ast.identifier.type = ast.ref.ref?.type;
    })
    .rule(qbscript.AssignStatement, (ast, { root, error }) => {
        if (!ast.target.ref) return;
        if (ast.target.ref.type.arr_length) {
            if (ast.target.index === undefined) {
                error(CST.first(ast.cst, '_group'), 'Cannot assign to array');    
                return
            }
        }
        try {
            check_assign(ast.target.ref.type, ast.source);
        }
        catch(e) {
            error(CST.first(ast.cst, 'value'), e);
        }
    })
    .rule(qbscript.HoldStatement, (ast, { root, error }) => {
        try {
            check_device(root, ast.device);
        }
        catch(e) {
            error(CST.first(ast.cst, 'identifier_device'), e);
        }
    })
    .rule(qbscript.ReleaseStatement, (ast, { root, error }) => {
        try {
            check_device(root, ast.device);
        }
        catch(e) {
            error(CST.first(ast.cst, 'identifier_device'), e);
        }
    })
    .rule(qbscript.IfStatement, (ast, { root, error }) => {
        const idx = root.statements.indexOf(ast);
        const next = root.statements[idx+1];
        if (!next) {
            error(CST.first(ast.cst, '_group'), 'Missing code for if statement');
            return
        }
    })
    .rule(qbscript.Reference, (ast, { root, error }) => {
        ast.ref = resolve_ref(root, ast, error);
        if (!ast.ref) return;
        if (typeof ast.index === 'number') {
            if (
                ast.index > ast.ref.type.arr_length-1
                || ast.index < -ast.ref.type.arr_length+1
            ) {
                error(CST.first(ast.cst, 'integer'), `Index out of bounds`);
            }
        }
    })
    .rule(qbscript.Identifier, (ast, { error }) => {
        if (ast.type) {
            const type = quimblos_types[ast.type.name];
            if (['void','err','ptr'].includes(ast.type.name as string) || !type) {
                error(CST.first(ast.cst, 'identifier_type'), `Invalid type '${ast.type.name}'`);
            }
        }
    })
    .rule(qbscript.TypeIdentifier, (ast, { error }) => {
        if (ast.arr_length !== undefined) {
            if (ast.arr_length <= 0) {
                error(CST.first(ast.cst, 'unsigned_integer'), `Array size must greater than 0`);
            }
        }
    })

/* Checkers */

function check_engine_device(name: string) {
    const device = qb.get_device(name);
    if (!device) {
        throw `Device '${name}' not found`
    }
}
function check_device(script: qbscript.Script, device_name: string) {
    const device = script.macros.find(m =>
        m instanceof qbscript.UseDeviceMacro && m.device === device_name
    );
    if (!device) {
        throw `Device '${device_name}' not imported, did you forget a "@use"?`
    }
    return qb.get_device(device_name);
}
function check_device_node(device: Device, node: string) {
    const port = device.nodes.findIndex(n => n.name === node);
    if (port < 0) {
        throw `Device '${device.name}' does not contain a node named '${node}'`
    }
    return device.nodes[port];
}
function check_script_node(script: qbscript.Script, node_name: string) {
    const node = script.declarations.find(d =>
        (d instanceof qbscript.VariableDeclaration
        || d instanceof qbscript.PointerDeclaration)
        && d.identifier.name === node_name
    );
    if (!node) {
        throw `Script does not contain a node named '${node_name}'`
    }
    return node;
}
function check_assign(target: qbscript.TypeIdentifier, source: qbscript.Value | qbscript.Reference) {
    const target_type = quimblos_types[target.name];
    if (!target_type) return;
    if (source instanceof qbscript.Value) {
        if (!target_type.allow_assign_value.includes(source.value_type)) 
            throw `A '${source.value_type}' value cannot be assigned to a '${target.name}' node`
    }
    else {
        if (source.ref) {
            if (!target_type.allow_assign_node.includes(source.ref.type.name)) 
                throw `A '${source.ref.type.name}' node cannot be assigned to a '${target.name}' node`
        }
    }
}
function check_index(type: qbscript.TypeIdentifier) {
    if (!['u8','i8','u16','i16','u32','i32'].includes(type.name))
        throw `A '${type}' node cannot be used as index`
}

/* Resolvers */

function resolve_ref(script: qbscript.Script, ref: qbscript.Reference, error: (cst: CSTNode, err: string) => void): qbscript.Identifier | undefined {
    let node;
    if (ref.device) {
        let device;
        try {
            device = check_device(script, ref.device);
        }
        catch (e) {
            error(CST.first(ref.cst,'identifier_device'), e)
            return
        }
        try {
            node = check_device_node(device, ref.node);
        }
        catch (e) {
            error(CST.first(ref.cst,'identifier'), e)
            return
        }
    }
    else {
        try {
            node = check_script_node(script, ref.node).identifier;
        }
        catch (e) {
            error(CST.first(ref.cst,'identifier'), e)
            return
        }
    }
    if (ref.index) {
        // TODO: check if it's array
        if (!node.type.arr_length) {
            error(CST.first(ref.cst, '_group'), `Index can only be used on arrays`)
            return;
        }
        if (ref.index instanceof qbscript.Reference) {
            const index_ref = resolve_ref(script, ref.index, error);
            if (index_ref) {
                try {
                    check_index(index_ref.type)
                }
                catch (e) {
                    error(CST.first(ref.cst,'ref'), e)
                    return
                }
            }
        }
    }
    return node;
}