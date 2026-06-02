import { Linter, CSTNode, CST } from "@quimblos/langmaker";
import { quimblos } from "./semantics";
import { quimblos_types } from "./types";
import { Driver, Kernel } from "../kernel";

export const make_quimblos_linter = (kernel: Kernel) => {
    
    /* Checkers */

    function check_engine_device(name: string) {
        const device = kernel.get_driver(name);
        if (!device) {
            throw `Device '${name}' not found`
        }
    }
    function check_device(script: quimblos.Script, device_name: string) {
        const device = script.macros.find(m =>
            m instanceof quimblos.UseDeviceMacro && m.device === device_name
        );
        if (!device) {
            throw `Device '${device_name}' not imported, did you forget a "@use"?`
        }
        return kernel.get_driver(device_name);
    }
    function check_device_node(device: Driver, ref: quimblos.Reference): quimblos.Identifier {
        const port = device.variables.findIndex(n => n.name === ref.node);
        if (port < 0) {
            throw `Device '${device.name}' does not contain a node named '${ref.node}'`
        }

        const identifier = new quimblos.Identifier();
        identifier.cst = ref.cst;
        identifier.name = ref.node;
        identifier.type = new quimblos.TypeIdentifier();
        identifier.type.cst = ref.cst;
        identifier.type.name = 'u8'; // TODO
        return identifier;
    }
    function check_script_node(script: quimblos.Script, ref: quimblos.Reference): quimblos.VariableDeclaration {
        const node = script.statements.find(d =>
            (d instanceof quimblos.VariableDeclaration)
            && d.identifier.name === ref.node
        );
        if (!node) {
            throw `Script does not contain a node named '${ref.node}'`
        }
        return node as any;
    }
    function check_assign(target: quimblos.TypeIdentifier, source: quimblos.Expression) {
        const target_type = quimblos_types[target.name];
        if (!target_type) return;

        const val = source.terms[0]!.value;

        if (val instanceof quimblos.Literal) {
            if (!target_type.allow_assign_literal.includes(val.literal_type)) 
                throw `A '${val.literal_type}' value cannot be assigned to a '${target.name}' node`
        }
        else if (val instanceof quimblos.Reference) {
            if (val.ref) {
                if (!target_type.allow_assign_node.includes(val.ref.type.name)) 
                    throw `A '${val.ref.type.name}' node cannot be assigned to a '${target.name}' node`
            }
        }
    }
    function check_index(type: quimblos.TypeIdentifier) {
        if (!['u8','i8','u16','i16','u32','i32'].includes(type.name))
            throw `A '${type}' node cannot be used as index`
    }

    /* Resolvers */

    function resolve_ref(script: quimblos.Script, ref: quimblos.Reference, error: (cst: CSTNode, err: string) => void): quimblos.Identifier | undefined {
        let node;
        if (ref.device) {
            let device;
            try {
                device = check_device(script, ref.device);
            }
            catch (e: any) {
                error(CST.first(ref.cst,'identifier_device')!, e)
                return
            }
            try {
                node = check_device_node(device, ref);
            }
            catch (e: any) {
                error(CST.first(ref.cst,'identifier')!, e)
                return
            }
        }
        else {
            try {
                node = check_script_node(script, ref).identifier;
            }
            catch (e: any) {
                error(CST.first(ref.cst,'identifier')!, e)
                return
            }
        }
        if (ref.index) {
            // TODO: check if it's array
            if (!node!.type.arr_length) {
                error(CST.first(ref.cst, '_group')!, `Index can only be used on arrays`)
                return;
            }
            if (ref.index instanceof quimblos.Reference) {
                const index_ref = resolve_ref(script, ref.index, error);
                if (index_ref) {
                    try {
                        check_index(index_ref.type)
                    }
                    catch (e: any) {
                        error(CST.first(ref.cst,'ref')!, e)
                        return
                    }
                }
            }
        }
        return node;
    }

    return new Linter<quimblos.Script>()
    
        // .rule(quimblos.UseDriverMacro, (ast, { error }) => {
        //     try {
        //         check_engine_device(ast.device);
        //     }
        //     catch(e: any) {
        //         error(CST.first(ast.cst, 'identifier_device')!, e);
        //     }
        // })
        .rule(quimblos.VariableDeclaration, (ast, { root, error }) => {
            // if (ast.value) {
            //     if (!ast.identifier) return;
            //     if (ast.identifier.type.arr_length) {
            //         error(CST.first(ast.cst, '_group')!, 'Cannot assign to array');
            //         return
            //     }
            //     try {
            //         check_assign(ast.identifier.type, ast.value);
            //     }
            //     catch(e: any) {
            //         error(CST.first(ast.cst, 'expression')!, e);
            //     }
            // }
        })
        .rule(quimblos.AssignStatement, (ast, { root, error }) => {
            // if (!ast.target.ref) return;
            // if (ast.target.ref.type.arr_length) {
            //     if (ast.target.index === undefined) {
            //         error(CST.first(ast.cst, '_group')!, 'Cannot assign to array');    
            //         return
            //     }
            // }
            // try {
            //     check_assign(ast.target.ref.type, ast.source);
            // }
            // catch(e: any) {
            //     error(CST.first(ast.cst, 'expression')!, e);
            // }
        })
        .rule(quimblos.HoldStatement, (ast, { root, error }) => {
            try {
                check_device(root, ast.device);
            }
            catch(e: any) {
                error(CST.first(ast.cst, 'identifier_device')!, e);
            }
        })
        .rule(quimblos.ReleaseStatement, (ast, { root, error }) => {
            try {
                check_device(root, ast.device);
            }
            catch(e: any) {
                error(CST.first(ast.cst, 'identifier_device')!, e);
            }
        })
        .rule(quimblos.IfStatement, (ast, { root, error }) => {
            const idx = root.statements.indexOf(ast);
            const next = root.statements[idx+1];
            if (!next) {
                error(CST.first(ast.cst, '_group')!, 'Missing code for if statement');
                return
            }
        })
        .rule(quimblos.Reference, (ast, { root, error }) => {
            const ref = resolve_ref(root, ast, error);
            if (!ref) return;
            ast.ref = ref;
        })
        .rule(quimblos.Identifier, (ast, { error }) => {
            if (ast.type) {
                const type = quimblos_types[ast.type.name];
                if (['void','err'].includes(ast.type.name as string) || !type) {
                    error(CST.first(ast.cst, 'identifier_type')!, `Invalid type '${ast.type.name}'`);
                }
            }
        })
        .rule(quimblos.TypeIdentifier, (ast, { error }) => {
            if (ast.arr_length !== undefined) {
                if (ast.arr_length <= 0) {
                    error(CST.first(ast.cst, 'unsigned_integer')!, `Array size must greater than 0`);
                }
            }
        })
}