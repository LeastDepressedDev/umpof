PROJECT = null

async function request_packs() {
    return (await fetch("%pack_rq%")).json()
}

function new_project() {

}

function create_new_node(nodeID) {

}

let lr_config = {
    name: "Test project 1",
    author: "qigan",
    using: [
        "defpack"
    ],
    nodes: [],
    ext_prefs: {}
}
function gen_project_base(config) {
    return lr_config // TODO: Change to config from gui from new_project()
}