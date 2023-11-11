function SetImage(imgs)
{
	var expandImg = document.getElementsByTagName("ArtAreaMainImage")[0];
	expandImg.innerHTML = "";
	
	IsVideo = imgs.src.includes("_thumb");
	ImagePath=imgs.src.replace("_small", "");
	
	if(imgs.src)
	{
		var str = "Visit Microsoft!";
		var res = str.replace("Microsoft", "W3Schools");
		
		if(IsVideo)
		{
			thing = document.createElement("video");
			thing.setAttribute("controls", "")
			expandImg.appendChild(thing);
			
			source = document.createElement("source");
			source.src = imgs.src.replace("_thumb.jpg", ".mp4");
			source.type = "video/mp4"
			//console.log(source.src);
			thing.appendChild(source);
			thing.className  = "GalleryMain";
		}
		else
		{
			thing = document.createElement("img");
			
			//link = document.createElement("a");
			//link.href="#img1"
			//
			//link2 = document.createElement("a");
			//link2.href="#"
			//link2.className ="lightbox"
			//link2.id="img1"
			//lightbox = document.createElement("span");
			//lightbox.style="background-image: url('" + ImagePath + "')"
			
			expandImg.appendChild(thing);
			//expandImg.appendChild(link);
			//link.appendChild(thing);
			//link2.appendChild(lightbox);
			//expandImg.appendChild(link2);
			
			thing.src = ImagePath
			thing.className  = "GalleryMain";
		}
		
		thing.onclick = function()
		{
			//alert(ImagePath)
			lightbox = document.createElement("span");
			fuck = document.createElement("div");
			thing2 = document.createElement("img");
			lightbox.appendChild(fuck);
			fuck.appendChild(thing2);
			document.body.appendChild(lightbox);
			thing2.src = ImagePath
			thing2.className ="lightboximage"
			lightbox.className ="lightbox"
			
			lightbox.onclick = function()
			{
				lightbox.remove()
			}
			lightbox.onclick = function()
			{
				lightbox.remove()
			}
		}
	}
}