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
            }
        ]
    }
]

if (document.addEventListener) {
    document.addEventListener('contextmenu', function(e) {
        document.getElementById("cm-container").innerHTML = "";
        create_context_menu(actual_m_pos[0], actual_m_pos[1], 50, main_context_menu, "true");

        e.preventDefault();
    }, false);
} else {
    document.attachEvent('oncontextmenu', function() {
        document.getElementById("cm-container").innerHTML = "";
        create_context_menu(actual_m_pos[0], actual_m_pos[1], 50, main_context_menu, "true");

        window.event.returnValue = false;
    });
}