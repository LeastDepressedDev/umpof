var PROJECT = null;
var NODLESS = [];

var SELECTED_LINK = null;

var eleidReverseMap = new Map();

async function request_packs() {
    return (await fetch("%pack_rq%")).json()
}

function new_project() {

}

function create_new_node(nodeID) {
    let ptbl_node_info = NODLESS[nodeID];
    return {
        uuid: crypto.randomUUID(),
        id: nodeID,
        ninf: ptbl_node_info,
        links: []
    }
}

function exclude_rebuild_list(list, ioob) {
    let m = [];
    for (let i = 0; i < list.length; i++) {
        if (i==ioob || list[i] == ioob) continue;
        m.push(list[i]);
    }
    return m;
}

function link(eleid1, eleid2) {
    let t1 = eleidReverseMap.get(eleid1);
    let t2 =  eleidReverseMap.get(eleid2);
    
    if (t1.type == t2.type) return;

   clock_link(t1, t2);



    console.log(t1);
    console.log(t2);
}

function clock_link(t1, k) {
    if (t1.type == "input") {
        if (t1.con != null) {
            let t = eleidReverseMap.get(t1.con);
            if (t.type != "output") {
                console.log("Something is fucked up... Sad wtf...");
            }
            t.con = exclude_rebuild_list(t.con, t1.eleid);
            console.log(t);
        }

        if (k.eleid == t1.con) {
            t1.con = null;
        } else {
            t1.con = k.eleid;
            k.con.push(t1.eleid);
        }
    } else if (t1.type == "output") {
        if (k.type != "input") return;

        let flag0 = false;
        t1.con.forEach(v => {
            if (v==k.eleid) flag0 = true;
        });

        if (flag0) {
            t1.con = exclude_rebuild_list(t1.con, k.eleid);
            k.con = null;
        } else {
            t1.con.push(k.eleid);
            k.con = t1.eleid;
        }
    }
}

function add_node_to_prj(node) {
    PROJECT.nodes.push(node);

    //
    let root = document.createElement("div");
    root.id = node.uuid;
    root.className = "dnode";

    let title = document.createElement("div");
    title.innerHTML = node.ninf.name;
    title.className = "dnode-title";
    root.appendChild(title);

    for (let i = 0; i < node.ninf.format.length; i++) {
        let form = node.ninf.format[i];

        let block = document.createElement("div");
        block.className = "dblock";
        if (form.block == "header") {
            block.innerHTML = `<p class="dnode-header"><b>${form.text}</b></p>`;
        } else {
            let tuid = crypto.randomUUID();
            block.innerHTML = `<p class="dnode-text">${form.text}<span class="dnode-ck-${form.block}" id="${tuid}"></span></p>`;
            var lk = {
                type: form.block,
                eleid: tuid,
                con: form.block == "output" ? [] : null
            };
            node.links.push(lk);
            eleidReverseMap.set(lk.eleid, lk);
        }
        
        root.setAttribute("mstate", "false");
        root.onmousedown = function (event) {root.setAttribute("mstate", "true")};
        root.onmouseup = function (event) {root.setAttribute("mstate", "false")};
        


        root.appendChild(block);
    }


    root.style = `left: ${actual_m_pos[0]}px; top: ${actual_m_pos[1]}px; width: ${node.ninf.width}px;`;

    document.getElementById("wsp").appendChild(root);

    for (let i = 0; i < node.links.length; i++) {
        let v = node.links[i];
        document.getElementById(v.eleid).onmousedown = function(event) {
            if (SELECTED_LINK == null) {
                SELECTED_LINK = v;
            } else {
                if (SELECTED_LINK == v) {
                    SELECTED_LINK = null;
                } else {
                    link(SELECTED_LINK.eleid, v.eleid);
                    SELECTED_LINK = null;
                }
            }
        }
    }

    //
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

function load_project(prj) {
    request_packs().then((a) => {
        let tz = new Map();
        a.forEach(c => tz.set(c.id, c));



        let flag0 = true;
        prj.using.forEach(v => {
            if (!tz.has(v)) flag0 = false;
        });

        if (flag0) {
                PROJECT = prj;
                
                let node_list = [];
                tz.forEach((v, k) => {
                    v.nodes.forEach(c => node_list.push(c));
                });
                
                NODLESS = node_list; //Change later to grouped map
                main_context_menu[0].val[0].val = [];

                for (let i  = 0; i < NODLESS.length; i++) {
                    let c = NODLESS[i];
                    main_context_menu[0].val[0].val.push({
                        title: c.name,
                        action: "fun",
                        val: function() {
                            add_node_to_prj(create_new_node(i));
                        }
                    });
                }
        } else {
            console.log("Not all packs are present... Failed to load project!")
        }





    })
}