import { validate } from "schema-utils";
import path from 'path';

const schema = {
  type: "object",
  properties: {
    test: {
      type: "string",
    },
  },
};

const template = (
  name,
  tag,
  content,
  style,
  ts,
  children,
  render
) => ''
+ `export class ${name} extends HTMLElement {\n`
+ `  connectedCallback() {\n`
+ `    this.innerHTML = '${content}';\n`
+ (style ? ''
+ `    const style = document.createElement("style");\n`
+ `    style.textContent = '${style}';\n`
+ `    this.appendChild(style);\n`
: '')
+ `    ${children}\n`
+ `    {${ts}}\n`
+ `  }\n`
+ `  render() {\n`
+ `    ${render}`
+ `  }\n`
+ `}\n`
+ ``
+ `customElements.define('${tag}', ${name});`

function capitalize(str) {
  let out = '';
  let cap = true;
  for (let i = 0; i < str.length; i++) {
    if (str[i] === '-') {
      cap = true;
    }
    else {
      if (cap) out += str[i].toUpperCase();
      else out += str[i];
      cap = false;
    }
  }
  return out;
}

function text_template(html) {
  let callbacks = [];
  const templates = [...html.matchAll(/{{(.*)}}/g)];
  for (const template of templates) {
    const id = `_v_${callbacks.length}`;
    html = html.slice(0, template.index) + `<a id="${id}"/>` + html.slice(template.index + template[0].length);
    callbacks.push({
      id, code: template[1].trim()
    })
  }

  const children = callbacks.map(cb => 
    `this.${cb.id} = this.querySelector("#${cb.id}");`
  ).join('\n');
  const render = callbacks.map(cb => 
    `this.${cb.id}.textContent = (() => ${cb.code})();`
  ).join('\n');
  return { html, children, render };
}

export default function (source) {
  const options = this.getOptions();
  validate(schema, options, {
    name: "Example Loader",
    baseDataPath: "options",
  });

  const filename = path.basename(this.resourcePath);
  const raw_name = filename.replace(/.wml$/,'');

  if (!filename.endsWith('.wml')) return;

  const _html = source.match(/<template>(.*)<\/template>/s)?.[1] ?? '';
  const _css = source.match(/<style>(.*)<\/style>/s)?.[1] ?? '';
  const ts = source.match(/<script>(.*)<\/script>/s)?.[1] ?? '';
    
  const name = capitalize(raw_name);
  const tag = raw_name;

  const tt = text_template(_html);

  const html = tt.html
    .replace(/\n/g,'\\n').replace(/'/g,'\\\'');
  const css = _css
    .replace(/\n/g,'\\n').replace(/'/g,'\\\'');

  const webc = template(name, tag, html, css, ts, tt.children, tt.render);

  console.log('---')
  console.log(webc);
  console.log('---')

  return webc;
}