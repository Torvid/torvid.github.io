//console.log('Hello Lightbox');
lightbox = null

window.addEventListener('hashchange', function()
{
	updateLightboxState();
})

function setHash(hash)
{
	//history.pushState(null,null,hash);
	//history.replaceState(null,null,hash);
}

function updateLightboxState()
{
	hash = window.location.hash;
	//console.log("location: " + hash)
	
	if(hash != "")
	{
		hash = hash.substring(1)
		path = document.getElementById(hash + "_Link").innerHTML;
		//console.log("path: " + path)
		text = document.getElementById(hash + "_Text").innerHTML;
		description = document.getElementById(hash + "_Description").innerHTML;
		source = document.getElementById(hash + "_Source").innerHTML;
		
		enableLightbox("/"+path, text, description, source)
	}
	else
	{
		if(lightbox)
		{
			lightbox.remove()
			history.pushState(null, null, '#');
		}
	}
}

updateLightboxState();

function enableLightbox(path, title, desc, source)
{
	if(path)
	{
		lightbox = document.createElement("span");
		lightbox.className ="lightboxGallery"
		
		if (path.endsWith(".mp4"))
		{
			video = document.createElement("video")
			videoSource = document.createElement("source")
			fuck = document.createElement("div")
			fuck.className = "fuck"
			video.className = "lightboximageGallery"
			video.setAttribute("controls", "")
			video.setAttribute("autoplay", "")
			video.setAttribute("loop", "")
			video.appendChild(videoSource);
			lightbox.appendChild(fuck);
			fuck.appendChild(video);

			isVideo = true;
			videoSource.src = "";
			videoSource.src = path;
			videoSource.type = "video/mp4";
			video.addEventListener("click", (event) => {event.stopPropagation();});
			videoSource.addEventListener("click", (event) => {event.stopPropagation();});
		}
		else
		{
			fuck = document.createElement("div")
			fuck.className = "fuck"
			thing2 = document.createElement("img");
			thing2.className = "lightboximageGallery"
			thing2.src = path
			lightbox.appendChild(fuck);
			fuck.appendChild(thing2);
			thing2.addEventListener("click", (event) => {event.stopPropagation();});
		}
		
		thing3 = document.createElement("div");
		paragraph = document.createElement("p");
		thing3.appendChild(paragraph)
		thing3.className = "lightboxtextGallery"
		paragraph.innerHTML = "<h2>" + title + "</h2>\n";
		paragraph.innerHTML += desc;
		paragraph.innerHTML += "<br/>";
		
		
		if (source)
			paragraph.innerHTML += "source: " + "<a href=\"" + source + "\">" + source + "</a>";
		
		lightbox.appendChild(thing3);
		
		paragraph.addEventListener("click", (event) => {event.stopPropagation();});

		fuck.onclick = function()
		{
			lightbox.remove()
			
			history.pushState(null, null, '#');
		}
		lightbox.onclick = function()
		{
			lightbox.remove()
			
			history.pushState(null, null, '#');
		}
		
		document.body.appendChild(lightbox);
	}
}