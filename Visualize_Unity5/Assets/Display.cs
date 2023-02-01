using UnityEngine;
using System.Collections;
using System.Text;
using System.IO;  
using System.Collections.Generic;

public class Display : MonoBehaviour {
 
	public string fileName;
	public GameObject sphere;
	private List<GameObject> aofSpheres;
	private int count;

	void Start()
	{
		Load (fileName);
	}

 private bool Load(string fileName)
 {
		count = 0;
		aofSpheres = new List<GameObject>();
     try
     {
         string line;
         // Create a new StreamReader, tell it which file to read and what encoding the file
         // was saved as
			print("si");
         StreamReader theReader = new StreamReader(fileName, Encoding.Default);
 		
         // Immediately clean up the reader after this block of code is done.
         // You generally use the "using" statement for potentially memory-intensive objects
         // instead of relying on garbage collection.
         
         using (theReader)
         {
             
             do
             {
                 line = theReader.ReadLine();
                     
                 if (line != null)
                 {
						string[] entries = line.Split(new char[0],System.StringSplitOptions.RemoveEmptyEntries);
						//print (entries[0]);
						string a = entries[0];
						print (entries.Length);
					if(count==0)
						{
							int size = int.Parse(a);
							for(int i = 0;i<size; i++)
							{
								GameObject atom = Instantiate (sphere) as GameObject;
								aofSpheres.Add(atom);
							}
							count++;
						}
					else {
                     if (entries.Length > 3)
						{
							//GameObject atom = Instantiate (sphere) as GameObject;
							
								aofSpheres[int.Parse(entries[0])-1].transform.position = new Vector3(float.Parse(entries[2]), float.Parse(entries[3]),float.Parse (entries[4]));
								if(entries[1].Equals("Ar"))
								{aofSpheres[int.Parse(entries[0])-1].GetComponent<Renderer>().material.color = Color.white;}
								else {aofSpheres[int.Parse(entries[0])-1].GetComponent<Renderer>().material.color = Color.red;}
							}
							count++;}
                 }
             }
             while (line != null);
 
             // Done reading, close the reader and return true to broadcast success    
             theReader.Close();
             return true;
             }
         }
 
         // If anything broke in the try block, we throw an exception with information
         // on what didn't work
         catch (IOException e)
         {
             Debug.Log(e.Message);
             return false;
         }
     }
 }