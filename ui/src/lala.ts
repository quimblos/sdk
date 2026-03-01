export class QbApp extends HTMLElement {
    connectedCallback() {
        this.innerHTML = '\n    <a class="bg">Quimblos App</a><br>\n';
        const style = document.createElement("style");
        style.textContent = '\n.bg {\n    background-color: #f00;\n}\n';
        this.appendChild(style);
    }
}
customElements.define('qb-app', QbApp);