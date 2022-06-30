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
			video.className = "lightboximageGallery"
			video.setAttribute("controls", "")
			video.setAttribute("autoplay", "")
			video.setAttribute("loop", "")
			video.appendChild(videoSource);
			lightbox.appendChild(video);

			isVideo = true;
			videoSource.src = "";
			videoSource.src = path;
			videoSource.type = "video/mp4";
		}
		else
		{
			thing2 = document.createElement("img");
			thing2.className = "lightboximageGallery"
			thing2.src = path
			lightbox.appendChild(thing2);
		}
		
		thing3 = document.createElement("div");
		thing3.className = "lightboxtextGallery"
		thing3.innerHTML = "<h2>" + title + "</h2>\n";
		thing3.innerHTML += desc;
		thing3.innerHTML += "<br/>";
		
		if (source)
			thing3.innerHTML += "source: " + "<a href=\"" + source + "\">" + source + "</a>";
		
		lightbox.appendChild(thing3);
		
		lightbox.onclick = function()
		{
			lightbox.remove()
			
			history.pushState(null, null, '#');
		}
		
		document.body.appendChild(lightbox);
	}
}