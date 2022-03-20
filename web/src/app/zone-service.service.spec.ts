import { TestBed } from '@angular/core/testing';

import { ZoneServiceService } from './zone-service.service';

describe('ZoneServiceService', () => {
  let service: ZoneServiceService;

  beforeEach(() => {
    TestBed.configureTestingModule({});
    service = TestBed.inject(ZoneServiceService);
  });

  it('should be created', () => {
    expect(service).toBeTruthy();
  });
});
