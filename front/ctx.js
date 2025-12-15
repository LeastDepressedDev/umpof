let main_context_menu = [
    {
        title: "New",
        action: "sub",
        w: 100,
        val: [
            {
                title: "Node",
                action: "sub",
                w: 100,
                val: [
                    
                ]
            },
            {
                title: "Event",
                action: "sub",
                w: 100,
                val: [
                    
                ]
            }
        ]
    }
]

var NAMED_CONTEXTS = {
    "file-bar": [
        {
            title: "New",
            action: "fun",
            val : () => new_project()
        },
        {
            title: "Load",
            action: "fun",
            val : function() {}
        },
        {
            title: "Save",
            action: "fun",
            val : function() {

            }
        },
        {
            title: "Options",
            action: "fun",
            val : () => createOptionsWindow(true)
        }
    ]
}

function constructCtxForNode(nodeId) {
    let ele = eleToNodeAddr.get(nodeId);
    document.getElementById(ele.uuid).setAttribute("mstate", "false");
    create_context_menu(actual_m_pos[0], actual_m_pos[1], 50, 
        [
            {
                title: "Remove",
                action: "fun",
                val: function() {
                    ele.links.forEach(v => {
                        if (v.con != null && v.con.length > 0) {
                            if (v.con instanceof Array) {
                                v.con.forEach(al => link(v.eleid, al));
                            } else link(v.eleid, v.con);
                        }

                        eleidReverseMap.delete(v.eleid);
                        eleToNodeAddr.delete(v.uuid);
                    });
                    
                    PROJECT.nodes=PROJECT.nodes.filter(vk => vk !== ele);
                    document.getElementById(ele.uuid).remove();
                }
            },
            {
                title: "Preference",
                action: "fun",
                val: function() {
                    gen_prefs_window_of_node(ele);
                }
            }
        ], "true");
}

function callContextFromIdBar(id) {
    let ele = document.getElementById(id);
    create_context_menu(ele.offsetLeft, ele.offsetTop+20, ele.offsetWidth, NAMED_CONTEXTS[id], "true");
    
}

function replaceDefaultCtx() {
    document.getElementById("cm-container").innerHTML = "";

    let ele = null;
    let nodes = document.getElementsByClassName("dnode");
    for (let i = 0; i < nodes.length; i++) {
        let tmp = nodes[i]
        if (actual_m_pos[0] >= tmp.offsetLeft && actual_m_pos[0] <= tmp.offsetLeft+tmp.offsetWidth && 
            actual_m_pos[1] >= tmp.offsetTop && actual_m_pos[1] <= tmp.offsetTop+tmp.offsetHeight) {
                ele = tmp;
                break;
            }
    }
    
    if (ele != null) { 
        constructCtxForNode(ele.id);
        return;
    }
    create_context_menu(actual_m_pos[0], actual_m_pos[1], 50, main_context_menu, "true");
}

if (document.addEventListener) {
    document.addEventListener('contextmenu', function(e) {
        e.preventDefault();
        if (PROJECT == null) return;
        replaceDefaultCtx();
    }, false);
} else {
    document.attachEvent('oncontextmenu', function() {
        window.event.returnValue = false;
        if (PROJECT == null) return;
        replaceDefaultCtx();
    });
}