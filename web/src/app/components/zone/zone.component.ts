import { Component, OnInit, Input, Output, EventEmitter} from '@angular/core';
import { ZoneItem } from 'src/app/zone-item';

@Component({
  selector: 'app-zone',
  templateUrl: './zone.component.html',
  styleUrls: ['./zone.component.scss']
})
export class ZoneComponent implements OnInit {
  @Input() zone!:ZoneItem;
  @Output() onToggleZone:EventEmitter<ZoneItem> = new EventEmitter();

  constructor() { }

  ngOnInit(): void {
  }

  onToggle(zone:ZoneItem) {
    this.onToggleZone.emit(zone);
  }
}
