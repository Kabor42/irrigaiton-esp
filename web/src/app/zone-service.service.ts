import { Injectable } from '@angular/core';
import { Observable } from 'rxjs';
import { ZoneItem } from './zone-item';
import {HttpClient, HttpHeaders} from "@angular/common/http";
import { tap } from 'rxjs';

const httpOptions = {
  headers: new HttpHeaders({'Content-Type':'application/json'})
}

@Injectable({
  providedIn: 'root'
})
export class ZoneServiceService {

  private apiUrl = "http://localhost:8123/zones"

  constructor(private http:HttpClient) { }

  toggleZone(zone:ZoneItem):Observable<ZoneItem> {
    const url = `${this.apiUrl}/${zone.id}`;
    return this.http.put<ZoneItem>(url,zone,httpOptions);
  }

  getZones():Observable<ZoneItem[]> {
    return this.http.get<ZoneItem[]>(this.apiUrl);
  }
}
