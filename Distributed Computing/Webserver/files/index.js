let cookie = ""
let memo_id = -1

window.onload = function () {
    // set cookie

    // set memo_id
}


function getAll() {
    let xml = new XMLHttpRequest()
    xml.withCredentials = true
    xml.open("GET","/api/memo", true)

    xml.onreadystatechange = function () {
        // on response, do this
        // xml.responseText is response body

        // response readt when state == 4 and status is 200
        if (this.readyState === 4 && this.status === 200) {
            // got a response, update with memos
            let ul = document.getElementById("memo-list")
            ul.innerHTML = "" // clear it
            let jdata = JSON.parse(this.responseText)

            for (const i in jdata) {
                //console.log(jdata[i])
                let li = document.createElement("li")
                li.className = "memo"
                let h3 = document.createElement("h3")
                h3.innerText = "<" + jdata[i]["id"] + ">"
                h3.innerText += "Memo: " + jdata[i]["memo"]
                let p = document.createElement("p")
                p.innerText = "Last-Edit: " + jdata[i]["last-edit"]
                li.appendChild(h3)
                li.appendChild(p)
                ul.appendChild(li)
            }


        }
    }

    xml.send()

}


function post() {
    let input = document.getElementById("input-memo").value;


    // create request
    let xml = new XMLHttpRequest()
    xml.withCredentials = true
    xml.open("POST","/api/memo", true)


    // async function on return
    xml.onreadystatechange = function () {
        // on response, do this
        // xml.responseText is response body

        // response readt when state == 4 and status is 200
        if(this.readyState === 4 && this.status === 200){
            // got a response, update with memos
            getAll()

        }

        console.log(xml.response)
        if(cookie === "") {
            // if there isn't a session cookie, need to grab one from the response
            cookie = xml.getResponseHeader("Cookie")
            document.cookie = cookie
        }
    }
    console.log(xml)
    xml.send(input)


}
