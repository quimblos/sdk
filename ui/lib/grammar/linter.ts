import { CST } from "../lang-maker/cst";
import { Linter } from "../lang-maker/linter";
import { qbscript } from "./semantics";
import { quimblos_types } from "./types";

function check_type_assign(target: qbscript.Type, source: qbscript.ValueType) {
    const target_type = quimblos_types[target];
    if (!target_type)
        return `Unknown type '${target}'`;
    if (!target_type.allow_assign_value.includes(source)) 
        return `A '${source}' value cannot be assigned to a '${target}' variable`
}


// private resolve_node(target, errors, script, error_at = 'target') {
//   const match = target.match(/^\s*(\S+?)(\.(\S+?))?(\[(\S+)\])?\s*$/);
//   if (!match) {
//     errors.add(error_at, 'Malformed target');
//     return
//   }

//   const [_, p0, __, p1, ___, p2] = match;
//   let device = undefined;
//   let node;
//   if (p0 in script.devices) {
//     if (!p1) {
//       errors.add(error_at, 'Must specify device node'); return
//     }
//     if (!(p1 in script.devices[p0].nodes)) {
//       errors.add(error_at, `Device '${p0}' has no node '${p1}'`); return
//     }
//     device = script.devices[p0];
//     node = device.nodes[p1];
//   }
//   else {
//     if (!(p0 in script.nodes)) {
//       errors.add(error_at, `Node '${p0}' not found on script`); return
//     }
//     if (p1) {
//       errors.add(error_at, `'${p0}' is not a device`); return
//     }
//     node = script.nodes[p0];
//   }

//   let index = undefined;
//   if (p2) {
//     if (node.type !== 'arr') {
//       errors.add(error_at, 'Index only allowed for array nodes'); return
//     }
//     index = parseInt(p2);
//     if (isNaN(idx)) {
//       errors.add(error_at, 'Index must be integer'); return
//     }
//     if (idx >= node.length) {
//       errors.add(error_at, 'Index out of bounds'); return
//     }
//   }
  
//   return {
//     device,
//     node,
//     index,
//     type: node.type
//   }
// }

// private resolve_type(type, val, errors, script, error_at = 'value') {
  
//   if (!(type in this.grammar.types)) {
//     errors.add('type', `Unknown type '${type}'`);
//     return;
//   }
//   if (val === undefined) return;

//   let val_type;
//   if (val === 'true' || val === 'false') val_type = 'bool';
//   else if (val[0] === '\'') {
//     if (val.at(-1) === '\'') val_type = 'str';
//     else {
//       errors.add('value', 'Unterminated string')
//       return;
//     }
//   }
//   else {
//     val = parseFloat(val);
//     if (!isNaN(val)) {
//       if (Number.isInteger(val)) {
//         if (val < 0) val_type = 'int';
//         else val_type = 'uint';
//       }
//       else val_type = 'float';
//     }
//     else {
//       const target = this.resolve_node(val, errors, script, error_at);
//       if (!target) return;
//       val_type = this.grammar.types[target.type].as_source;
//     }
//   }

//   const allow_cast_from = this.grammar.types[type].allow_cast_from;
//   if (!allow_cast_from.includes(val_type)) {
//     errors.add(error_at, `Cannot cast ${val_type} to ${type}`);
//     return;
//   }
//   return { value: val, type: val_type };
// }

export const quimblos_linter = new Linter()
    
    .rule(qbscript.UseDeviceMacro, (node, { error }) => {
        const device = qb.get_device(node.device);
        if (!device) {
            const identifier_cst = CST.first(node.cst, 'identifier');
            error(identifier_cst, `Device '${node.device}' not found`);
        }
    })

    .rule(qbscript.Identifier, (node, { error }) => {
        const type = quimblos_types[node.type];
        if (node.type as string === 'void' || !type) {
            const identifier_cst = CST.first(node.cst, 'type_identifier');
            error(identifier_cst, `Invalid type '${node.type}'`);
        }
    })

    .rule(qbscript.VariableDeclaration, (node, { error }) => {
        if (node.value) {
            const type_error = check_type_assign(node.identifier.type, node.value.value_type);
            if (type_error) {
                const value_cst = CST.first(node.cst, 'value');
                error(value_cst, type_error);
            }
        }
    })