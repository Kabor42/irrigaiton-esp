import { Component, OnInit } from '@angular/core';
import { ZoneItem } from 'src/app/zone-item';
import { ZoneServiceService } from 'src/app/zone-service.service';

@Component({
  selector: 'app-zone-display',
  templateUrl: './zone-display.component.html',
  styleUrls: ['./zone-display.component.scss']
})
export class ZoneDisplayComponent implements OnInit {
  zones:ZoneItem[] = [];

  constructor(private zoneService:ZoneServiceService) { }

  ngOnInit(): void {
    this.zoneService.getZones().subscribe((z)=>{z.forEach((item)=>(this.zones.push(item)))});
    console.log(this.zones);
  }

  toggleZone(zone:ZoneItem) {
    zone.active = !zone.active;
    this.zoneService.toggleZone(zone)
      .subscribe();

  }

}
